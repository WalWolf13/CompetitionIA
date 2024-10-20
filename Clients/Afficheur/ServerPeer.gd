extends Node

signal fini

signal packet_received
signal connection_successful

var stream : StreamPeerTCP
var lastPacket : PackedByteArray

var status = 0
var fin = false
func _process(_delta):
	if(stream == null):
		return
	stream.poll()
	var new_status = stream.get_status()
	if(status != new_status):
		if(new_status == StreamPeerTCP.STATUS_CONNECTED):
			connection_successful.emit()
		else:
			return
	if(fin == false):
		var ab = stream.get_available_bytes()
		
		if(ab > 0):
			lastPacket = stream.get_data(ab)[1]
			if(ab == 1 && lastPacket.decode_u8(0) == 255):
				print("Server exited")
				fin = true
				emit_signal("fini")
				self.queue_free()
			emit_signal("packet_received", lastPacket)
		elif(ab == -1):
			fin = true
			emit_signal("fini")
func getFin():
	return fini

func connect_to(host : String, port : int):
	stream = StreamPeerTCP.new()
	stream.connect_to_host(host, port)
	await(connection_successful)
	var err = stream.put_data(PackedByteArray("VISIO".to_ascii_buffer()))
	if(err != OK):
		printerr("Send error : ", err)
	await packet_received
	if(lastPacket.get_string_from_ascii() != "VOK"):
		printerr("server returned weird shit : ", lastPacket.get_string_from_ascii())
		self.queue_free()
	else:
		print("Connected")

var player_count = 0
func get_player_count() -> int:
	stream.put_8(3)
	await packet_received
	player_count = lastPacket.decode_u8(0)
	return player_count

func get_player_positions() -> Array[PlayerTransform]:
	var data : PackedByteArray = []
	var ret : Array[PlayerTransform] = []
	stream.put_8(4)
	var i = 0
	while(i < player_count * 10):
		await packet_received
		i+=lastPacket.size()
		data.append_array(lastPacket)
	
	i = 0
	for p in player_count:
		var pt = PlayerTransform.new()
		pt.position = Vector2(data.decode_u32(i), data.decode_u32(i+4))
		pt.angle = ((data.decode_u16(i+8)-32768.)/(10430.))
		i += 10
		ret.append(pt)
	
	return ret

func get_wall_count() -> int:
	stream.put_8(1)
	await packet_received
	return lastPacket.decode_u8(0)

func get_walls() -> PackedInt32Array:
	var data : PackedInt32Array = []
	var wall_count = (await get_wall_count())
	stream.put_8(2)
	var i = 0
	while(i < wall_count*16):
		await packet_received
		i+=lastPacket.size()
		data.append_array(lastPacket.to_int32_array())
	
	return data

#func get_walls() -> PackedInt32Array:
	#stream.put_8(2)
	#await packet_received
	#return lastPacket.to_int32_array()

func start_race():
	stream.put_8(5)
