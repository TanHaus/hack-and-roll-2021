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


Tone.Transport.start();
pattern.start(0);



function updateMusic(data) {
	// update music with new data
	console.log(data);
}