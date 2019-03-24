goog.require('Blockly.Blocks')
goog.require('Blockly.Colours')

Blockly.Blocks['start'] = {
  init: function() {
    this.jsonInit({
      message0: '%1',
      args0: [
        {
          type: 'field_image',
          src: Blockly.mainWorkspace.options.pathToMedia + 'start.svg',
          width: 40,
          height: 40
        }
      ],
      inputsInline: true,
      nextStatement: null,
      category: Blockly.Categories.event,
      colour: '#696969'
    })
  }
}

Blockly.Blocks['run'] = {
  init: function() {
    this.jsonInit({
      message0: '%1 %2',
      args0: [
        {
          type: 'field_image',
          src: Blockly.mainWorkspace.options.pathToMedia + 'run.svg',
          width: 40,
          height: 40
        },
        {
          type: 'input_value',
          name: 'TIMES',
          check: 'Number'
        }
      ],
      inputsInline: true,
      previousStatement: null,
      nextStatement: null,
      category: Blockly.Categories.event,
      colour: '#e0e0e0'
    })
  }
}

Blockly.Blocks['repeat'] = {
  init: function() {
    this.jsonInit({
      message0: '%3 %2 %1',
      args0: [
        {
          type: 'input_statement',
          name: 'SUBSTACK'
        },
        {
          type: 'field_image',
          src: Blockly.mainWorkspace.options.pathToMedia + 'repeat.svg',
          width: 40,
          height: 40
        },
        {
          type: 'input_value',
          name: 'TIMES',
          check: 'Number'
        }
      ],
      inputsInline: true,
      previousStatement: null,
      nextStatement: null,
      category: Blockly.Categories.control,
      colour: Blockly.Colours.control.primary
    })
  }
}

Blockly.Blocks['left'] = {
  init: function() {
    this.jsonInit({
      message0: '%1',
      args0: [
        {
          type: 'field_image',
          src: Blockly.mainWorkspace.options.pathToMedia + 'left.svg',
          width: 40,
          height: 40
        }
      ],
      inputsInline: true,
      previousStatement: null,
      nextStatement: null,
      category: Blockly.Categories.event,
      colour: '#5ecef3'
    })
  }
}

Blockly.Blocks['right'] = {
  init: function() {
    this.jsonInit({
      message0: '%1',
      args0: [
        {
          type: 'field_image',
          src: Blockly.mainWorkspace.options.pathToMedia + 'right.svg',
          width: 40,
          height: 40
        }
      ],
      inputsInline: true,
      previousStatement: null,
      nextStatement: null,
      category: Blockly.Categories.event,
      colour: '#5ecef3'
    })
  }
}

Blockly.Blocks['klaxon'] = {
  init: function() {
    this.jsonInit({
      message0: '%1',
      args0: [
        {
          type: 'field_image',
          src: Blockly.mainWorkspace.options.pathToMedia + 'klaxon.svg',
          width: 40,
          height: 40
        }
      ],
      inputsInline: true,
      previousStatement: null,
      nextStatement: null,
      category: Blockly.Categories.event,
      colour: '#ff9b9b'
    })
  }
}
