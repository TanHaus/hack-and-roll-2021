let synth, analyser, pattern;

function setup() {
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
	
	pattern = new Tone.Pattern(function(time, note){
		synth.triggerAttackRelease(note, "4n");
		}, ["C4", "D4", "E4", "A3"], "upDown");
		//https://tonejs.github.io/docs/r12/CtrlPattern
	
	pattern.loop = true;
	pattern.interval = "8n";
	
	analyser = new AudioEnergy();
	synth.connect(analyser);
	synth.connect(Tone.Master);
	
	Tone.Transport.start();
	pattern.start(0);



  createCanvas(710, 400);
  noFill();
}

function draw() {
	background(0);

	const dim = Math.min(width, height);

	strokeWeight(dim * 0.0175);
  stroke(255);
  noFill();
	
	analyser.update(deltaTime);

	strokeWeight(dim * 0.0025);
  stroke(255);
  noFill();

  beginShape();
  const energies = analyser.getEnergyBins();

  for (let i = 0; i < energies.length; i++) {
    const energy = energies[i];
    const x = map(i, 0, energies.length - 1, 0, width);
    const offset = map(energy, -100, -30, 0, 1);
    const y = height - offset * height;
    vertex(x, y);
  }
  endShape();
}


function updateMusic(data) {
	// update music with new data
	console.log(data);
}