let aX = 0, aY = 0, aZ = 0, gA = 0.5, gB = 0.5, gC = 0.5;

// Master volume in decibels
const baseVolume = -20;
// maxVolume = 5;
const volumeMultiplier = 20;

const pitchChange = 0;
const pitchShiftThreshold = 0.3;

const beatThreshold = 0.2;

const MP3 = "Doja Cat - Say So (Audio).mp3";

const adornment1Link = "Tambourine Single Hit 1 SOUND Effect.mp3";
const adornment2Link = "BABABOOEY Sound Effect.mp3"

// The synth we'll use for audio
let player;

let playing = false;


// let autoFilter;
let pitchShift;
let autoPanner;
let feedbackDelay;

let adornment1, adornment2;
let playAdornment1 = true, playAdornment2 = true;

// Create a new canvas to the browser size
async function setup() {
  createCanvas(windowWidth, windowHeight);

  // Make the volume quieter
  Tone.Master.volume.value = baseVolume;

  // We can use 'player' to play MP3 files
  player = new Tone.Player();
  player.loop = true;
  player.autostart = false;
  player.loopStart = 1.0;

  // Load and "await" the MP3 file
  await player.load(MP3);

  // Wire up connections
	autoFilter = new Tone.AutoFilter("8n");
	autoFilter.wet.value = 0; // between 0 and 1
	autoFilter.start();
	
	pitchShift = new Tone.PitchShift();

	autoPanner = new Tone.AutoPanner("4n");
	autoPanner.frequency.value = 15;

	analyser = new AudioEnergy();

	player.chain(analyser, pitchShift, Tone.Master);
	// player.chain(autoPanner, Tone.Master);

	adornment1 = new Tone.Player();
	adornment1.loop = false;
	adornment1.autostart = false;
	await adornment1.load(adornment1Link);
	adornment1.volume.value = 5;
	adornment1.chain(Tone.Master);

	adornment2 = new Tone.Player();
	adornment2.loop = false;
	adornment2.autostart = false;
	await adornment2.load(adornment2Link);
	adornment2.volume.value = 5;
	adornment2.chain(Tone.Master);
}

// On window resize, update the canvas size
function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
}

// Render loop that draws shapes with p5
function draw() {
  if (!player || !player.loaded) {
    // MP3 not loaded
    return;
  }
	
	pitchShift.pitch = 12*Math.sqrt(aX*aX + aY*aY)/0.5;


	Tone.Master.volume.value = baseVolume + aZ*volumeMultiplier;
	// console.log(Tone.Master.volume.value)

	if (aZ > 0.67 + pitchShiftThreshold || aZ < 0.67 - pitchShiftThreshold) {
		pitchShift.pitch = pitchChange;
	}

	if (Math.abs(aY) > beatThreshold) {
		if (playAdornment1) {
			playAdornment1 = false;
			adornment1.start();
			// console.log("hello1");
			setTimeout(() => playAdornment1 = true, 50);
		}
	}

	if (Math.abs(aX) > beatThreshold) {
		if (playAdornment2) {
			playAdornment2 = false;
			adornment2.start();
			// console.log("hello2");
			setTimeout(() => playAdornment2 = true, 50);
		}
	}

  const dim = Math.min(width, height);
	
	  // Black background
		background(0);

		strokeWeight(dim * 0.0175);
		stroke(255);
		noFill();
	
		// Update the analyser
		analyser.update(deltaTime);
	
		// Draw FFT values
		strokeWeight(dim * 0.0025);
		// stroke(255);
		stroke(gA*255, gB*255, gC*255);
		// fill(< 30 ? 255 * Math.pow(1.1, -t) : 0);
	
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
	
		// Draw a 'play' or 'stop' button
		noStroke();
		fill(255);
		polygon(
			width / 2,
			height / 2,
			dim * 0.1,
			playing ? 4 : 3,
			playing ? PI / 4 : 0
		);
		
}

// Update the FX and trigger synth ON
function mousePressed() {
  if (player && player.loaded) {
    if (player.state === "started") {
      player.stop();
    } else {
      player.start();
    }
  }
}

function mousePressed() {
  if (player && player.loaded) {
    if (playing) {
      playing = false;
      player.stop();
    } else {
      playing = true;
      player.restart();
    }
  }
}

// Draw a basic polygon, handles triangles, squares, pentagons, etc
function polygon(x, y, radius, sides = 3, angle = 0) {
  beginShape();
  for (let i = 0; i < sides; i++) {
    const a = angle + TWO_PI * (i / sides);
    let sx = x + cos(a) * radius;
    let sy = y + sin(a) * radius;
    vertex(sx, sy);
  }
  endShape(CLOSE);
}

function updateMusic(data) {
	aX = data['aX'];
	aY = data['aY'];
	aZ = data['aZ'];
	gX = data['gX'];
	gY = data['gY'];
	gZ = data['gZ'];
}