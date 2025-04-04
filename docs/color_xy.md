### XY color

Part of the [CIE 1931 color space](https://en.wikipedia.org/wiki/CIE_1931_color_space)

The XYZ color space is designed, so Y is the luminosity of the color.

The xyY color space uses xy coordinates and brightness.

The values are fractions of 1 (0 <= X,Y,Z <= 1.0)

Color gamut is asumed as red(X=1.0,Y=0.0), green(X=0.0,Y=1.0), blue(X=0.0,Y=0.0)

Philips hue uses D65 wide RGB, so we asume this to be standard.

#### XYZ color space

The values are 0 <= X,Y,Z <= 1.0

#### xyY color space

The color is specified by xy and brightness

The formulas expect the values to be fractions of 1 (0.0 ... 1.0)
x = X / (X + Y + Z)
y = Y / (X + Y + Z)
z = Z / (X + Y + Z) = 1 - x - y

Y = brightness (0 <= Y <= 1.0)
X = (Y / y) * x
Z = (Y / y) * z = (Y / y) * (1 - x - y)

##### convert RGB to XYZ

- take RGB and convert to fractions of 1
 - `r,g,b = 1.0f * (R,G,B / 255)` [R,G,B 0...255]
- apply gamma correction
 - `if (r,g,b > 0.04045) r,g,b = pow( (r + 0.055) / (1.0 + 0.055) , 2.4 )`
 - `else r,g,b = r,g,b / 12.92`
- convert to XYZ
 - float X = red * 0.649926f + green * 0.103455f + blue * 0.197109f;
 - float Y = red * 0.234327f + green * 0.743075f + blue * 0.022598f;
 - float Z = red * 0.000000f + green * 0.053077f + blue * 1.035763f;

##### convert RGB to xyY

- convert to XYZ
- calculate xy values from XYZ
 - x = X / (X + Y + Z)
 - y = Y / (X + Y + Z)
 - brightness = Y
- check coordinates are within given gamut
- find closest point within gamut

##### convert xyY to RGB

- check coordinates are within given gamut
- find closest point within gamut
- convert to XYZ
 - x,y are given
 - z = 1.0 - x - y
 - Y = brightness
 - X = (Y / y) * x
 - Z = (Y / y) * z
- convert to RGB (Wide gamut RGB conversion matrix)
 - float r = X * 1.4628067f - Y * 0.1840623f - Z * 0.2743606f;
 - float g = -X * 0.5217933f + Y * 1.4472381f + Z * 0.0677227f;
 - float b = X * 0.0349342f - Y * 0.0968930f + Z * 1.2884099f;
- apply (reverse) gamma correction
 - r = r <= 0.0031308f ? 12.92f * r : (1.0f + 0.055f) * pow(r, (1.0f / 2.4f)) - 0.055f;
 - g = g <= 0.0031308f ? 12.92f * g : (1.0f + 0.055f) * pow(g, (1.0f / 2.4f)) - 0.055f;
 - b = b <= 0.0031308f ? 12.92f * b : (1.0f + 0.055f) * pow(b, (1.0f / 2.4f)) - 0.055f;

#### color gamut

specified in 3 xy point coordinates for red, green and blue

if gamut is unknown we assume very wide, idealistic gamut of red(X=1.0,Y=0.0), green(X=0.0,Y=1.0), blue(X=0.0,Y=0.0)

#### color temperature

[Correlated Color Temperature](https://en.wikipedia.org/wiki/Correlated_color_temperature)

The color temperature conversion points lie along a curved line within the CIE 1931 xy coordinates.
black body locus or [Planckian locus](https://en.wikipedia.org/wiki/Planckian_locus)
MacAdam ellipse 3-step and/or 5-step

The approximation formulas are at the WikiPedia article.

Values from LUXEON 5050 LED data sheet

| kelvin |   x    |   y    |
| ------ | ------ | ------ |
| 1800 K | 0.5493 | 0.4083 |
| 2200 K | 0.5018 | 0.4153 |
| 2700 K | 0.4578 | 0.4101 |
| 3000 K | 0.4338 | 0.4030 |
| 3500 K | 0.4073 | 0.3917 |
| 4000 K | 0.3818 | 0.3797 |
| 5000 K | 0.3447 | 0.3558 |
| 5700 K | 0.3287 | 0.3417 |
| 6500 K | 0.3123 | 0.3282 |

### RGB color

#### compute luminance from RGB

I like the formula `int Y = ((R << 1) + R + (G << 2) + B) >> 3;`.
It's sufficiently accurate and considerably fast, for lighting appliances.

Mostly luminance is computed as `Y = (R + G + B) / 3`.
This is not how the human eye would perceive it.

There are performance formulas for the ITU BT.601 formula.
`Y = (R+R + G+G+G + B) / 6`
`Y = (R+R+R + G+G+G+G + B) >> 3`

The ITU BT.601 gives the formula `Y = 0.299 R + 0.587 G + 0.114 B`.
This gives more weight to red and green.
More like the human perceives the light.

The ITU BT.709 states the formula `Y = 0.2125 R + 0.7154 G + 0.0721 B`

Check the data sheet of your RGB LEDs for the actual brightness parameters and work your own formula from there.

My testing LED stripe is equipped with Samsung 5050 RGB LEDs.
The data sheet lists following parameters.

| Color | brightness    | wave length |
| ----- | ------------- | ----------- |
| RED   |  600- 800 mcd |  620-630 nm |
| GREEN | 1500-1800 mcd |  520-530 nm |
| BLUE  |  400- 600 mcd |  460-470 nm |
