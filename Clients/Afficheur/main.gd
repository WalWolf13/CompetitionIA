extends Node2D
var state = 0
var count = 0

func start_race():
	if(state == 1):
		return
	
	var newcount = await $ServerPeer.get_player_count()
	for i in newcount-count:
		var c = preload("res://car.tscn").instantiate()
		$Cars.add_child(c)
	count = newcount
	state = 1
	$CanvasLayer2.visible = false;
	$CanvasLayer3.visible = true;
	$ServerPeer.start_race()
	
	await $ServerPeer.getFin()
	print("FIN !")
	state = 0
	get_tree().change_scene_to_file("res://SceneDeFin.tscn")

func _process(_delta):
	if(state == 1):
		move_cars()

var vehiculeRegarder = 0;
func change_vehicule():
	if(state == 1):
		vehiculeRegarder += 1
		if(vehiculeRegarder == $ServerPeer.player_count):
			vehiculeRegarder = 0

func move_cars():
	var i = 0
	for pt in await $ServerPeer.get_player_positions():
		var c = $Cars.get_child(i) as Node2D
		c.position = pt.position
		c.global_rotation = pt.angle
		if(i == vehiculeRegarder):
			$Camera2D.position = pt.position
		i = i + 1

func build_map(data : PackedInt32Array):
	print("Mapping")
	var walls : Array[Line2D] = [] 
	var i = 0
	
	while (i < data.size()):
		var vect_start = Vector2(data[i], data[i+1])
		var vect_end = Vector2(data[i+2], data[i+3])
		i += 4
		
		for w in walls:
			if(w.points[w.points.size()-1] == vect_start):
				w.add_point(vect_end)
				continue
		
		var w = preload("res://wall.tscn").instantiate()
		w.add_point(vect_start)
		w.add_point(vect_end)
		walls.append(w)
		$Walls.add_child(w)


func connect_button():
	$CanvasLayer.visible = false
	await $ServerPeer.connect_to($CanvasLayer/LineEdit.text, $CanvasLayer/LineEdit2.text.to_int())
	build_map(await $ServerPeer.get_walls())
	$CanvasLayer2.visible = true
