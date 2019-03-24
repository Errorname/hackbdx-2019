const functions = require('firebase-functions')

const admin = require('firebase-admin')
admin.initializeApp()

exports.currentProgram = functions.https.onRequest((req, res) => {
  return admin
    .database()
    .ref('/current_program')
    .once('value')
    .then(snapshot => {
      return res.json(snapshot.val())
    })
})

exports.setCurrentProgram = functions.https.onRequest((req, res) => {
  const stack = req.query.stack.split('|').map(block => {
    let splitted = block.split('-')
    if (splitted[1] === 'repeat') {
      return {
        id: splitted[0],
        name: splitted[1],
        type: splitted[2],
        count: splitted[2] === 'start' ? Number(splitted[3]) : 0
      }
    } else if (splitted[1] === 'run') {
      return {
        id: splitted[0],
        name: splitted[1],
        count: Number(splitted[2])
      }
    } else if (!['start', 'left', 'right', 'klaxon'].includes(splitted[1])) {
      return res.send('ERROR')
    }
    return { id: splitted[0], name: splitted[1] }
  })

  admin
    .database()
    .ref('/current_program')
    .set(stack)

  admin
    .database()
    .ref('/current_instruction')
    .set('0')

  admin
    .database()
    .ref('/current_loop_count')
    .set(0)
  res.send('OK')

  return res.send('OK')
})

exports.nextAction = functions.https.onRequest((req, res) => {
  return admin
    .database()
    .ref('/')
    .once('value')
    .then(snapshot => {
      let { current_instruction, current_loop_count, current_program } = snapshot.val()

      if (!current_instruction) {
        return res.send('wait')
      } else {
        const program = stackToProgram(current_program)

        const next = nextInstruction(program, current_instruction, current_loop_count)

        if (next) {
          admin
            .database()
            .ref('/current_instruction')
            .set(next.id)

          if (next.name === 'repeat') {
            return res.send('wait')
          } else {
            return res.send(next.name + (next.count ? `-${next.count}` : ''))
          }
        } else {
          admin
            .database()
            .ref('/current_instruction')
            .set(false)

          return res.send('wait')
        }
      }
    })
})

exports.start = functions.https.onRequest((req, res) => {
  admin
    .database()
    .ref('/current_instruction')
    .set('0')
  admin
    .database()
    .ref('/current_loop_count')
    .set(0)
  res.send('OK')
})

exports.sync = functions.https.onRequest((req, res) => {
  const previous = req.query.previous

  admin
    .database()
    .ref('/current_instruction')
    .on('value', snapshot => {
      let current = snapshot.val()

      if (current !== previous) {
        res.send(current)
      }
    })
})

// HELPERS

const stackToProgram = stack => {
  stack = parseRepeat(stack)

  // Add next
  stack.reduce((previous, el) => {
    if (!previous) return el

    previous.next = el

    if (el.substack) {
      el.first = el.substack[0]
      el.substack.reduce((subPrev, subEl) => {
        if (!subPrev) return subEl

        subPrev.next = subEl

        return subEl
      }, null)
      el.substack[el.substack.length - 1].next = el
    }

    return el
  }, null)

  return { instructions: stack }
}

const nextInstruction = (program, currentInstruction, currentLoopCount) => {
  if (currentInstruction) {
    current = findInstructionById(program, currentInstruction)
    if (current.first && currentLoopCount < current.count) {
      admin
        .database()
        .ref('/current_loop_count')
        .set(currentLoopCount + 1)
      return current.first
    } else {
      return current.next
    }
  }
  return null
}

const findInstructionById = (program, id) => {
  for (inst of program.instructions) {
    if (inst.id === id) return inst
    if (inst.substack) {
      for (subinst of inst.substack) {
        if (subinst.id === id) return subinst
      }
    }
  }
}

const parseRepeat = stack => {
  const repeat_start = stack.find(el => el.name === 'repeat' && el.type === 'start')

  if (repeat_start) {
    const repeat_end = stack.find(el => el.name === 'repeat' && el.type === 'end')

    if (repeat_end) {
      const repeat_start_index = stack.indexOf(repeat_start)
      const repeat_end_index = stack.indexOf(repeat_end)

      if (repeat_start_index < repeat_end_index) {
        const substack = stack.splice(repeat_start_index + 1, repeat_end_index - repeat_start_index)
        substack.pop()
        repeat_start.substack = substack
      } else {
        throw new Error('Error code 2')
      }
    } else {
      throw new Error('Error code 1')
    }
  }

  return stack
}
