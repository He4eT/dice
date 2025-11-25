module.exports = [
  {
    "type": "text",
    "defaultValue": "Press your watch&rsquo;s right side button to&nbsp;instantly roll a&nbsp;random number from 1&nbsp;to&nbsp;N."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Dice Settings"
      },
      {
        "messageKey": "UPPER_BOUND",
        "type": "slider",
        "label": "N",
        "defaultValue": 6,
        "step": 1,
        "min": 1,
        "max": 999999,
        "attributes": {
          "style": "display: none;"
        }
      },
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];