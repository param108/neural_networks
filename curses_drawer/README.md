## To Build
`make draw`

## To generate new data
`./draw save <letter that you are saving>`

## To use
- left click to place a mark
- right click to remove the mark
- 's' to save and start new
- 'q' to quit (does not save)
- 'e' to erase current marks and start again.
- 'm' toggle mode
		* CONTINUOUS - left click to start drawing, move mouse to draw, left click again to stop. Similarly for right click and erase.
		* CLICK - left click to make one mark. right click to erase one mark.

The data will be written as a json array `[[[x,y],[x1,y1]....],"<letter>"]` (where x,y are coordinates of the marked points) to a file `saved.json`. If the file already exists it will be appended.

## To view saved data
`./draw read <filename>`


## Saved Json Format
```
[[[x1,y1],[x2,y2]...],"a"],
....
....

```

to make the file a proper json file you will need to add a `[` at the beginning and `]` at the end (remember to remove the last `,`
