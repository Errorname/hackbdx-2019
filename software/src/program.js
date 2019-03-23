const runProgram = program => {
  program.current = null
  program.currentLoopCount = 0

  const runNext = () => {
    if (program.current) {
      unglowBlock(program.current.id)
    }

    nextInstruction(program)

    if (program.current) {
      glowBlock(program.current.id)
      setTimeout(runNext, 500)
    }
  }
  runNext()
}

const nextInstruction = program => {
  if (program.current) {
    if (program.current.first && program.currentLoopCount < program.current.count) {
      program.currentLoopCount++
      program.current = program.current.first
    } else {
      program.current = program.current.next
    }
  } else {
    program.current = program.instructions[0]
  }
}

const glowBlock = blockId => app.workspace.glowBlock(blockId, true)
const unglowBlock = blockId => app.workspace.glowBlock(blockId, false)

/*const glowBlock = blockId => app.workspace.glowBlock(blockId, true)
const unglowBlock = blockId => app.workspace.glowBlock(blockId, false)

const glowAll = () => {
  const ids = ['a', 'b', 'c', 'd', 'e', 'f', 'g']
  let current = 0
  setInterval(() => {
    unglowBlock(ids[(current - 1 + ids.length) % ids.length])
    glowBlock(ids[current % ids.length])
    current++
  }, 200)
}*/
