const app = {}

const main = () => {
  app.workspace = Blockly.inject('blocklyDiv', {
    comments: false,
    disable: false,
    collapse: false,
    media: 'icons/',
    readOnly: false,
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

  //printStack([{ name: 'start' }])
  printStack([
    { id: 'a', name: 'start' },
    { id: 'b', name: 'repeat', type: 'start', count: 3 },
    { id: 'c', name: 'run', count: 3 },
    { id: 'd', name: 'left', count: '45°' },
    { id: 'e', name: 'run', count: 2 },
    { id: 'f', name: 'right', count: '90°' },
    { name: 'repeat', type: 'end' },
    { id: 'g', name: 'klaxon' }
  ])
}

const printStack = stack =>
  Blockly.Xml.domToWorkspace(Blockly.Xml.textToDom(stackToXml(stack)), app.workspace)

const glowBlock = blockId => app.workspace.glowBlock(blockId, true)
const unglowBlock = blockId => app.workspace.glowBlock(blockId, false)

const glowAll = () => {
  const ids = ['a', 'b', 'c', 'd', 'e', 'f', 'g']
  let current = 0
  setInterval(() => {
    unglowBlock(ids[(current - 1 + ids.length) % ids.length])
    glowBlock(ids[current % ids.length])
    current++
  }, 200)
}

const currentId = 0

const stackToXml = stack => {
  stack = stack.map(el => ({ ...el }))

  stack = parseRepeat(stack)

  const xml = {
    xml: {
      '@xmlns': 'http://www.w3.org/1999/xhtml',
      variables: {},
      block: iterateBlocks(null, stack)
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
    block['@x'] = 100
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
