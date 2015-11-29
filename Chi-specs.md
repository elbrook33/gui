# Chi

## API
- ui-open [path]
- creates: pid/{control,events,left,middle,right}

### events
- Click left/middle/right line # char # x # y #
- Command command
- Key key (if not captured by command)

### control
- Color:{left/middle/right}/{0-4}
- Commands:<special-key>key-name=Command name;<key2>… (Note two spaces between commands.)
- Redraw:[left/middle/right]
- Quit


## Markup

((fade))
[[accent]]
normal

_italic_
**bold**
normal

| highlight
|| active
||| selected

<block lines=# text="">
<image file="">
<video file="">

Bg colors: 0% 2% 10% | 70% (90%) (100%)
Text colors: 100% 70% 30% | 0% 20% (50%)
Highlights: 10% 50% | 20% (50%) | (50%) (10%)


## Implementation
- SDL(?) for framebuffer GL context.
- nanovg for drawing, including text.
- …which leaves:
	* managing contexts
	* parsing markup
	* making draw calls
	* handling events
