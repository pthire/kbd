{
  "targets": [
    {
      "target_name": "kbd",
      "sources": [
        "kbd.cc",
        "kbd_f.cc"
      ],
      "include_dirs": ["<!(node -e \"require('nan')\")"]
    }
  ]
}
