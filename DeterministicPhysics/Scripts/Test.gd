extends Node

func _ready():
	# Call the calculate function when the node is ready
	calculate()

func calculate():
	# Assuming physicsD is a class you defined or a built-in class.
	var s = physicsD.new()
	s.increase()
	var a = s.getNumber()
	print("The value of a is: ", a)
