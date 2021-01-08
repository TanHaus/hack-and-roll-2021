let rootNode;
let depth;

const nodes = [],
			rt3 = Math.sqrt(3);

let synth;

synth = new Tone.PolySynth({
  // "volume": -10,
  "envelope": {
    "attack": 0.5,
    "decay": 0,
    "sustain": 0.3,
    "release": 0,
    }
}).toMaster();

synth.set({"oscillator": {
          "type": "sine" 
					}
});

var pattern = new Tone.Pattern(function(time, note){
  synth.triggerAttackRelease(note, "4n");
  }, ["C4", "D4", "E4", "A3"], "upDown");
	//https://tonejs.github.io/docs/r12/CtrlPattern

pattern.loop = true;
pattern.interval = "8n";
// pattern.loopEnd = "32n";

function setup() {
	createCanvas(windowWidth, screen.availHeight, WEBGL);
  setAttributes('antialias', true);
	createEasyCam({distance:260});
	// suppress right-click context menu
  document.oncontextmenu = function() { return false; };
	strokeWeight(2);
	
	rootNode = new Node(0, 0, 0, 1);
	nodes.push(rootNode);
	rootNode.addChildren(6);


	Tone.Transport.start();
  pattern.start(0);
}

function song(t) {
	bassSynth.triggerAttackRelease(1000, '8n', t)
	console.log(t)
}

function draw() {
	background(0);
	rotateY(frameCount*.004);
	if (frameCount % 360 === 60) rootNode.stimulate();
	rootNode.draw();
}