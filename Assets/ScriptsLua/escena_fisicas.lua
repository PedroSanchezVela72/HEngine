entities = { "MainLight","Floor", "Collider", "Trigger", "Camera"}

MainLight = {
	Transform = {
		position = {x = 0, y = 10, z = 10}
	},
	Light = {
		type = "Directional",
		diffuseColor = {r = 1, g = 1, b = 1},
		specularColor = {r = 1, g = 1, b = 1},
	}
}

Floor = {
	
	Transform = {
		position = {x = 0, y = 0, z = 0}
		
	},
	RigidBody = {
		mass = 0,
		rbStatic = true,
		rbKinematic = false
	},
	BoxCollider = {
		halfExtents = {x = 20, y = 0.5, z = 20},
		draw = true,
		trigger = false,
		positionOffset = {x = 0, y = 0, z = 0}
	}
}


Collider = {
	Transform = {
		position = {x = 0, y = 15, z = 0},
		rotation = {x = 1, y = 0, z = 0, w = 0}
	},
	RigidBody = {
		mass = 3,
		rbStatic = false,
		rbKinematic = false
	},
	SphereCollider = {
		ratius = 2.5,
		draw = true,
        trigger = false,
		positionOffset = {x = 0, y = 2.5, z = 0}
	}
}



Trigger = {
	Transform = {
		position = {x = 0, y = 3, z = 0},
		rotation = {x = 1, y = 0, z = 0, w = 0}
	},
	RigidBody = {
		mass = 0,
		rbStatic = true,
		rbKinematic = false
	},
	BoxCollider = {
		halfExtents = {x = 5, y = 2, z = 5},
		draw = true,
        trigger = true,
		positionOffset = {x = 0, y = 0, z = 0}
	}
}

Cubo3= {
	Parent = "Cubo1",
	Transform = {
		position = {x = 20, y = 0, z = 0},
		rotation = {x = 1, y = 0, z = 0, w = 0}
	},
	RenderMesh = {
		mesh = "Cube.4380.mesh"
	}
}

Camera = {
	Transform = {
		position = {x = 0, y = 5, z = 60}
	},
	CameraComponent = {
		mainCamera = true,
		name = "CameraH",
		lookAtTarget = true,
		showCursor = true,
		nearClipDistance = 5,
		farClipDistance = 200,
		target = {x = 0, y = 5, z = 0},
		backgroundColor = {r = 0.1, g = 0.1, b = 0.1}
	}
}