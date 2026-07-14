graph [
	 directed 1
	 hierarchic 1

 node [
	 id 0
	 label "0             []_-1"
 	graphics [
	 type "ellipse"
	 hasFill 0
	 w 90.0   h 110.0
	 outline "#000000" ]
]
 	LabelGraphics [
	 text "0             []_-1"
	 fontSize 12
	 fontName "Dialog"
	 model "sides"
	 anchor "e"
	 borderDistance -50.0
]

 node [
	 id 1
	 label "1             [0]_0"
 	graphics [
	 type "ellipse"
	 hasFill 0
	 w 90.0   h 110.0
	 outline "#000000" ]
]
 	LabelGraphics [
	 text "1             [0]_0"
	 fontSize 12
	 fontName "Dialog"
	 model "sides"
	 anchor "e"
	 borderDistance -50.0
]

 node [
	 id 2
	 label "2             [1]_1"
 	graphics [
	 type "ellipse"
	 hasFill 0
	 w 90.0   h 110.0
	 outline "#000000" ]
]
 	LabelGraphics [
	 text "2             [1]_1"
	 fontSize 12
	 fontName "Dialog"
	 model "sides"
	 anchor "e"
	 borderDistance -50.0
]

 node [
	 id 3
	 label "3             [2]_2"
 	graphics [
	 type "ellipse"
	 hasFill 0
	 w 90.0   h 110.0
	 outline "#000000" ]
]
 	LabelGraphics [
	 text "3             [2]_2"
	 fontSize 12
	 fontName "Dialog"
	 model "sides"
	 anchor "e"
	 borderDistance -50.0
]

 node [
	 id 4
	 label "4             [0, 2]_2"
 	graphics [
	 type "ellipse"
	 hasFill 0
	 w 90.0   h 110.0
	 outline "#000000" ]
]
 	LabelGraphics [
	 text "4             [0, 2]_2"
	 fontSize 12
	 fontName "Dialog"
	 model "sides"
	 anchor "e"
	 borderDistance -50.0
]

 node [
	 id 5
	 label "5             [1, 2]_2"
 	graphics [
	 type "ellipse"
	 hasFill 0
	 w 90.0   h 110.0
	 outline "#000000" ]
]
 	LabelGraphics [
	 text "5             [1, 2]_2"
	 fontSize 12
	 fontName "Dialog"
	 model "sides"
	 anchor "e"
	 borderDistance -50.0
]

 node [
	 id 6
	 label "6             [0, 2]_0"
 	graphics [
	 type "ellipse"
	 hasFill 0
	 w 90.0   h 110.0
	 outline "#000000" ]
]
 	LabelGraphics [
	 text "6             [0, 2]_0"
	 fontSize 12
	 fontName "Dialog"
	 model "sides"
	 anchor "e"
	 borderDistance -50.0
]

 node [
	 id 7
	 label "7             []_-1"
 	graphics [
	 type "ellipse"
	 hasFill 0
	 w 90.0   h 110.0
	 outline "#000000" ]
]
 	LabelGraphics [
	 text "7             []_-1"
	 fontSize 12
	 fontName "Dialog"
	 model "sides"
	 anchor "e"
	 borderDistance -50.0
]

edge [
	 source 0
	target 1
	label "0"
	graphics [
	fill "#000000" 		targetArrow "standard"	 	 
]]

edge [
	 source 0
	target 2
	label "1"
	graphics [
	fill "#000000" 		targetArrow "standard"	 	 
]]

edge [
	 source 0
	target 3
	label "2"
	graphics [
	fill "#000000" 		targetArrow "standard"	 	 
]]

edge [
	 source 1
	target 4
	label "2"
	graphics [
	fill "#000000" 		targetArrow "standard"	 	 
]]

edge [
	 source 2
	target 5
	label "2"
	graphics [
	fill "#000000" 		targetArrow "standard"	 	 
]]

edge [
	 source 3
	target 6
	label "0"
	graphics [
	fill "#000000" 		targetArrow "standard"	 	 
]]

edge [
	 source 4
	target 7
	label "1"
	graphics [
	fill "#000000" 		targetArrow "standard"	 	 
]]

edge [
	 source 5
	target 7
	label "0"
	graphics [
	fill "#000000" 		targetArrow "standard"	 	 
]]

edge [
	 source 6
	target 7
	label "1"
	graphics [
	fill "#000000" 		targetArrow "standard"	 	 
]]
]
