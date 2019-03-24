app.car = {
  x: 0,
  y: 10,
  direction: 180
}

const carRun = (car, count) => {
  const delta = 30 * count

  switch (car.direction) {
    case 180:
      car.y += delta
      break
    case 90:
      car.x -= delta
      break
    case 0:
      car.y -= delta
      break
    case 270:
      car.x += delta
      break
  }

  refreshCar(car)
}

const carRotate = (car, left = true) => {
  if (left) car.direction -= 90
  else car.direction += 90

  refreshCar(car)
}

const refreshCar = car => {
  const style = document.querySelector('#road img').style
  style.bottom = car.y + 'px'
  style.left = car.x + 'px'
  style.transform = `rotate(${car.direction}deg)`
}

const carUpdate = (car, instructionId) => {
  if (instructionId === false) return

  const instruction = findInstructionById(app.program, instructionId)

  switch (instruction.name) {
    case 'run':
      carRun(car, instruction.count)
      break
    case 'left':
      carRotate(car, true)
      break
    case 'right':
      carRotate(car, false)
      break
    case 'klaxon':
      alert('BIP BIP!')
      break
    case 'start':
      app.car.x = 0
      app.car.y = 10
      app.car.direction = 180
      refreshCar(app.car)
      break
  }
}
