void WalkBaseParams() {
  uint8_t paramToChange = random8(8);
  switch(paramToChange) {
    case 0:
      baseParams.dimSpeed = 127 - random8();
      break;
    case 1:
      baseParams.colorSpeed = 127 - random8();
      break;
    case 2:
      baseParams.spacing = random8();
      break;
    case 3:
      baseParams.colorThickness = random8();
      break;
    case 4:
      baseParams.brightLength = random8();
      break;
    case 5:
      baseParams.transLength = random8();
      break;
    case 6:
      baseParams.numColors = random8();
      break;
    case 7:
      baseParams.displayMode = random8();
      break;
    default:
      break;
  }

  PrintBaseParams();
}

void WalkTopParams() {
  PrintTopParams();
}

void WalkPaletteManagerParams() {
  PrintPaletteManagerParams();
}

void WalkLayerParams() {
  PrintLayerParams();
}

