module.exports = [
  {
    "type": "heading",
    "defaultValue": "Remind Me"
  },
  {
    "type": "text",
    "defaultValue": "Welcome to your personal 'Keep in Mind' watchface. This app is designed to help you stay focused on your goals, habits, or positive thoughts. By displaying your custom reminders directly under the time, you subconsciously process them every time you check the watch."
  },
  {
    "type": "section",
    "items": [
      { "type": "heading", "defaultValue": "Tutorial" },
      { "type": "text", "defaultValue": "Enter up to three different reminders below. You can cycle through them on your watch by either **shaking your wrist** or **tapping the screen**. The font size will automatically adjust to fit your text perfectly." }
    ]
  },
  {
    "type": "section",
    "items": [
      { "type": "heading", "defaultValue": "Your Reminders" },
      { "type": "input", "messageKey": "CONFIG_REMINDER_1", "defaultValue": "Keep Smiling", "label": "First Reminder", "attributes": { "maxlength": 48 } },
      { "type": "input", "messageKey": "CONFIG_REMINDER_2", "defaultValue": "Drink Water", "label": "Second Reminder", "attributes": { "maxlength": 48 } },
      { "type": "input", "messageKey": "CONFIG_REMINDER_3", "defaultValue": "Stay Focused", "label": "Third Reminder", "attributes": { "maxlength": 48 } }
    ]
  },
  {
    "type": "section",
    "items": [
      { "type": "heading", "defaultValue": "Top Area & Time" },
      { "type": "color", "messageKey": "CONFIG_COLOR_TOP", "defaultValue": "0x55AAFF", "label": "Top Background Color" },
      { "type": "color", "messageKey": "CONFIG_COLOR_TIME", "defaultValue": "0xFF0000", "label": "Time Color" },
      { "type": "color", "messageKey": "CONFIG_COLOR_BATTERY", "defaultValue": "0xFF0000", "label": "Battery Bar Color" },
      { "type": "toggle", "messageKey": "CONFIG_OUTLINE_WHITE", "label": "White Outline (Time)", "defaultValue": false, "description": "Enable this for better readability on dark top colors." },
      { "type": "toggle", "messageKey": "CONFIG_HIDE_ICON", "label": "Hide Center Icon", "defaultValue": false, "description": "Hides the brain icon for a cleaner, minimalist look." }
    ]
  },
  {
    "type": "section",
    "items": [
      { "type": "heading", "defaultValue": "Reminder Field" },
      { "type": "color", "messageKey": "CONFIG_COLOR_TEXT", "defaultValue": "0x000055", "label": "Reminder Text Color" },
      { "type": "color", "messageKey": "CONFIG_COLOR_BOTTOM", "defaultValue": "0xFFFFAA", "label": "Background (Start Color)" },
      { "type": "toggle", "messageKey": "CONFIG_ENABLE_GRADIENT", "label": "Enable Smooth Gradient", "defaultValue": false },
      { "type": "color", "messageKey": "CONFIG_COLOR_BOTTOM_END", "defaultValue": "0x55AAFF", "label": "Gradient End Color" }
    ]
  },
  { "type": "submit", "defaultValue": "Save Settings" },
  {
    "type": "text",
    "defaultValue": "Made with love by <a href='https://atomlabor.de'>https://atomlabor.de</a>",
    "attributes": { "style": "text-align: center; font-size: 12px; color: #888;" }
  }
];