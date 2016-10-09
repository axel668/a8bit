module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Configuration for a8bit watch face"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Settings"
      },
      {
        "type": "select",
        "messageKey": "ColorScheme",
        "defaultValue": "CGA",
        "label": "Color Scheme",
        "capabilities": ["COLOR"],
        "options": [          
          { 
            "label": "CGA (Palette1)",
            "value": "CGA" 
          },
          { 
            "label": "Monochrome", 
            "value": "MON" 
          },
          { 
            "label": "Mono Green",
            "value": "GRE" 
          },
          { 
            "label": "Mono Amber",
            "value": "AMB" 
          },
          { 
            "label": "Commodore",
            "value": "C64" 
          },
          { 
            "label": "Gameboy",
            "value": "GAB" 
          }
        ]
      },
      {
        "type": "toggle",
        "messageKey": "ShowHealthDefault",
        "label": "Show health as default",
        "defaultValue": false,
        "capabilities": ["HEALTH"]
      }
      ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];