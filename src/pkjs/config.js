module.exports = [
  {
    "type": "heading",
    "defaultValue": "Remember Me"
  },
  {
    "type": "text",
    "defaultValue": "Welcome to your personal 'Keep in Mind' watchface. This app is designed to help you stay focused on your goals, habits, or positive thoughts. By displaying your custom reminders directly under the time, you'll subconsciously process them every time you check the watch."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Tutorial"
      },
      {
        "type": "text",
        "defaultValue": "Enter up to three different reminders below. You can cycle through them on your watch by either **shaking your wrist** or **tapping the screen**."
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Your Reminders"
      },
      {
        "type": "input",
        "messageKey": "CONFIG_REMINDER_1",
        "defaultValue": "Keep Smiling",
        "label": "First Reminder",
        "attributes": { "maxlength": 64 }
      },
      {
        "type": "input",
        "messageKey": "CONFIG_REMINDER_2",
        "defaultValue": "Drink Water",
        "label": "Second Reminder",
        "attributes": { "maxlength": 64 }
      },
      {
        "type": "input",
        "messageKey": "CONFIG_REMINDER_3",
        "defaultValue": "Stay Focused",
        "label": "Third Reminder",
        "attributes": { "maxlength": 64 }
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Design & Colors"
      },
      {
        "type": "color",
        "messageKey": "CONFIG_COLOR_TOP",
        "defaultValue": "0x55AAFF",
        "label": "Top Background Color",
        "sunlight": true
      },
      {
        "type": "toggle",
        "messageKey": "CONFIG_OUTLINE_WHITE",
        "label": "Use White Outline for Time",
        "defaultValue": false
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];