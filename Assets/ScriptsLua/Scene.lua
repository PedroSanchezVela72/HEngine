entities = { "Camera","CanvasUI","Cube","Cube1","Cube2","Cube3","Cube4","Cube5","Listener","MainLight","Muros_Collection","Particula","chair","prueba"}

Camera = {
  CameraComponent = {
    farClipDistance = 200,
    mainCamera = true,
    showCursor = true,
    target = {
      z = 0,
      y = 0,
      x = 0,
    },
    name = "CameraH",
    lookAtTarget = true,
    nearClipDistance = 5,
    backgroundColor = {
      g = 0.10000000149012,
      r = 0.10000000149012,
      b = 0.10000000149012,
    },
  },
  Transform = {
    position = {
      z = 20,
      y = 0,
      x = 7,
    },
  },
}

CanvasUI = {
  Canvas = {
    movable = true,
    resizable = true,
  },
  Transform = {
    position = {
      z = 0,
      y = 0,
      x = 0,
    },
  },
}

Cube = {
  Parent = "Muros_Collection",
  BoxCollider = {
    halfExtents = {
      z = 0.75,
      y = 2.0,
      x = 2.0,
    },
    trigger = false,
    draw = false,
    positionOffset = {
      z = 0,
      y = 2.0,
      x = 0,
    },
  },
  RigidBody = {
    mass = 0.0,
    rbKinematic = true,
    rbStatic = false,
  },
  Transform = {
    position = {
      z = 5.9775094985962,
      y = 0.0,
      x = 8.058198928833,
    },
    rotation = {
      z = -4.3846624606175e-25,
      y = 1.1102230246252e-16,
      x = 3.9493528447565e-09,
    },
    scale = {
      z = 1.0,
      y = 1.0,
      x = 1.0,
    },
  },
  RenderMesh = {
    mesh = "Cube.4380.mesh",
  },
}

Cube1 = {
  Parent = "Muros_Collection",
  BoxCollider = {
    halfExtents = {
      z = 0.75,
      y = 2.0,
      x = 2.0,
    },
    trigger = false,
    draw = false,
    positionOffset = {
      z = 0,
      y = 2.0,
      x = 0,
    },
  },
  RigidBody = {
    mass = 0.0,
    rbKinematic = false,
    rbStatic = true,
  },
  Transform = {
    position = {
      z = -0.0,
      y = 0.0,
      x = -7.988302230835,
    },
    rotation = {
      z = 0.0,
      y = 0.0,
      x = 0.0,
    },
    scale = {
      z = 1.0,
      y = 1.0,
      x = 1.0,
    },
  },
  AudioSource = {
    mode = "3D",
    velocity = {
      z = 0,
      y = 0,
      x = 0,
    },
    volume = 0.80000001192093,
    filePaths = {
      filePath0 = "ruidoblanco.mp3",
    },
    numSounds = 1,
    soundNames = {
      soundName0 = "ruidoblanco",
    },
  },
  RenderMesh = {
    mesh = "Cube.4380.mesh",
  },
}

Cube2 = {
  Parent = "Muros_Collection",
  BoxCollider = {
    halfExtents = {
      z = 0.75,
      y = 2.0,
      x = 2.0,
    },
    trigger = false,
    draw = false,
    positionOffset = {
      z = 0,
      y = 2.0,
      x = 0,
    },
  },
  RigidBody = {
    mass = 0.0,
    rbKinematic = false,
    rbStatic = true,
  },
  Transform = {
    position = {
      z = 1.2465410232544,
      y = 0.0,
      x = 1.3109999895096,
    },
    rotation = {
      z = -0.0,
      y = 0.70710676908493,
      x = 0.0,
    },
    scale = {
      z = 1.0,
      y = 1.0,
      x = 1.0,
    },
  },
  RenderMesh = {
    mesh = "Cube.4380.mesh",
  },
}

Cube3 = {
  Parent = "Muros_Collection",
  BoxCollider = {
    halfExtents = {
      z = 0.75,
      y = 2.0,
      x = 2.0,
    },
    trigger = false,
    draw = false,
    positionOffset = {
      z = 0,
      y = 2.0,
      x = 0,
    },
  },
  RigidBody = {
    mass = 0.0,
    rbKinematic = false,
    rbStatic = true,
  },
  Transform = {
    position = {
      z = 5.2269577980042,
      y = 0.0,
      x = -10.689319610596,
    },
    rotation = {
      z = -0.0,
      y = 90.0,
      x = 0.0,
    },
    scale = {
      z = 1.0,
      y = 1.0,
      x = 1.0,
    },
  },
  RenderMesh = {
    mesh = "Cube.4380.mesh",
  },
}

Cube4 = {
  Parent = "Muros_Collection",
  BoxCollider = {
    halfExtents = {
      z = 0.75,
      y = 2.0,
      x = 2.0,
    },
    trigger = false,
    draw = false,
    positionOffset = {
      z = 0,
      y = 2.0,
      x = 0,
    },
  },
  RigidBody = {
    mass = 0.0,
    rbKinematic = false,
    rbStatic = true,
  },
  Transform = {
    position = {
      z = 9.2089805603027,
      y = 0.0,
      x = -10.689319610596,
    },
    rotation = {
      z = -0.0,
      y = 90.0,
      x = 0.0,
    },
    scale = {
      z = 1.0,
      y = 1.0,
      x = 1.0,
    },
  },
  RenderMesh = {
    mesh = "Cube.4380.mesh",
  },
}

Cube5 = {
  Parent = "Muros_Collection",
  BoxCollider = {
    halfExtents = {
      z = 0.75,
      y = 2.0,
      x = 2.0,
    },
    trigger = false,
    draw = false,
    positionOffset = {
      z = 0,
      y = 2.0,
      x = 0,
    },
  },
  RigidBody = {
    mass = 0.0,
    rbKinematic = false,
    rbStatic = true,
  },
  Transform = {
    position = {
      z = -0.0,
      y = 0.0,
      x = 0.0,
    },
    rotation = {
      z = 0.0,
      y = 0.0,
      x = 0.0,
    },
    scale = {
      z = 1.0,
      y = 1.0,
      x = 1.0,
    },
  },
  RenderMesh = {
    mesh = "Cube.4380.mesh",
  },
}

Listener = {
  AudioListener = {
    velocity = {
      z = 0,
      y = 0,
      x = 0,
    },
    up = {
      z = 0,
      y = 1,
      x = 0,
    },
    forward = {
      z = 1,
      y = 0,
      x = 0,
    },
  },
  Transform = {
    position = {
      z = 0,
      y = 0,
      x = 0,
    },
  },
}

MainLight = {
  Light = {
    type = "Directional",
    specularColor = {
      g = 1,
      r = 1,
      b = 1,
    },
    diffuseColor = {
      g = 1,
      r = 1,
      b = 1,
    },
  },
  Transform = {
    position = {
      z = 10.0,
      y = 10.0,
      x = 0.0,
    },
  },
}

Muros_Collection = {
  Parent = "prueba",
  Transform = {
    position = {
      z = 0,
      y = 0,
      x = 0,
    },
    
  },
}

Particula = {
  RenderParticle = {
    rotationSpeedRangeEnd = 0,
    position = {
      z = 0,
      y = 0,
      x = 0,
    },
    scaleRate = 0,
    sorted = true,
    width = 10,
    direction = {
      z = 0,
      y = 1,
      x = 0,
    },
    rotationRangeStart = 0,
    image = "smokecolors.png",
    minVelocity = 60,
    rotationSpeedRangeStart = 0,
    rotationRangeEnd = 0,
    material = "Smoke",
    maxVelocity = 80,
    quota = 100,
    cullEach = true,
    angle = 30,
    emmisionRate = 10,
    height = 10,
    timeToLive = 5,
    addImageAffector = true,
  },
  Transform = {
    position = {
      z = 0,
      y = 0,
      x = 40,
    },
  },
}

chair = {
  Parent = "Muros_Collection",
  BoxCollider = {
    halfExtents = {
      z = 0.43484500050545,
      y = 0.49546399712563,
      x = 0.37329000234604,
    },
    trigger = false,
    draw = false,
    positionOffset = {
      z = 0,
      y = 0.49546399712563,
      x = 0,
    },
  },
  RigidBody = {
    mass = 0.0,
    rbKinematic = false,
    rbStatic = true,
  },
  Transform = {
    position = {
      z = 4.7962322235107,
      y = 0.40496200323105,
      x = 0.0,
    },
    rotation = {
      z = -0.0,
      y = 0.0,
      x = 0.0,
    },
    scale = {
      z = 1.0,
      y = 1.0,
      x = 1.0,
    },
  },
  RenderMesh = {
    mesh = "Cube.4279.mesh",
  },
}

prueba = {
  Transform = {
    position = {
      z = 0,
      y = 0,
      x = 0,
    },
  },
}

