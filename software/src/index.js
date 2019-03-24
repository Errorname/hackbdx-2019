const app = {}

const main = () => {
  app.workspace = Blockly.inject('blocklyDiv', {
    comments: false,
    disable: false,
    collapse: false,
    media: 'icons/',
    readOnly: true,
    rtl: false,
    horizontalLayout: false,
    sounds: false,
    grid: { spacing: 16, length: 1, colour: '#2C344A', snap: false },
    zoom: {
      startScale: 2.0,
      controls: false,
      wheel: false
    }
  })

  setProgram([{ id: 'a', name: 'start' }])
  /*setProgram([
    { name: 'start' },
    { name: 'repeat', type: 'start', count: 3 },
    { name: 'run', count: 3 },
    { name: 'left' },
    { name: 'run', count: 2 },
    { name: 'right' },
    { name: 'repeat', type: 'end' },
    { name: 'klaxon' }
  ])*/
}

const setProgram = stack => {
  app.workspace.clear()
  app.program = stackToProgram(stack)
  Blockly.Xml.domToWorkspace(Blockly.Xml.textToDom(programToXml(app.program)), app.workspace)
}

const stackToProgram = stack => {
  stack = stack.map(el => ({ ...el }))

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

const programToXml = program => {
  const xml = {
    xml: {
      '@xmlns': 'http://www.w3.org/1999/xhtml',
      variables: {},
      block: iterateBlocks(null, program.instructions)
    }
  }

  return xmlbuilder.create(xml, null, null, { headless: true }).end({ pretty: true })
}

const iterateBlocks = (current, rest) => {
  if (!current) {
    ;[current, ...rest] = rest
    current = blockToXml(current)
  }

  if (rest.length > 0) {
    ;[next, ...rest] = rest
    next = blockToXml(next)
    current.next = { block: next }
    iterateBlocks(next, rest)
  }

  return current
}

const blockToXml = el => {
  let block = {
    '@type': el.name,
    '@id': el.id
  }

  if (el.name == 'start') {
    block['@x'] = 130
    block['@y'] = 100
  }

  if (el.count) {
    block.value = {
      '@name': 'TIMES',
      shadow: {
        '@type': 'math_whole_number',
        field: {
          '@name': 'NUM',
          '#text': el.count
        }
      }
    }
  }

  if (el.substack) {
    block.statement = {
      '@name': 'SUBSTACK',
      block: iterateBlocks(null, el.substack)
    }
  }

  return block
}

const parseRepeat = stack => {
  const repeat_start = stack.find(el => el.name == 'repeat' && el.type == 'start')

  if (repeat_start) {
    const repeat_end = stack.find(el => el.name == 'repeat' && el.type == 'end')

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

const setCurrentInstruction = (app, id) => {
  console.log(id)
  if (app.currentInstruction) {
    glowBlock(app.currentInstruction, false)
  }
  app.currentInstruction = id
  if (app.currentInstruction) {
    glowBlock(app.currentInstruction)
  }
}

const glowBlock = (blockId, toggle = true) => app.workspace.glowBlock(blockId, toggle)
