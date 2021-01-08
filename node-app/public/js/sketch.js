// let rootNode;
// let depth;

// const nodes = [],
// 			rt3 = Math.sqrt(3);


let objects = []
let mic, fft;
let dataDiv;

function setup() {
	// createCanvas(windowWidth, screen.availHeight, WEBGL);
	createCanvas(windowWidth, windowHeight);
  // setAttributes('antialias', true);
	// createEasyCam({distance:260});
	// // suppress right-click context menu
  // document.oncontextmenu = function() { return false; };
	// strokeWeight(2);
	
	// rootNode = new Node(0, 0, 0, 1);
	// nodes.push(rootNode);
	// rootNode.addChildren(6);



	

  mic = new p5.AudioIn();
  mic.start();
  fft = new p5.FFT();
  fft.setInput(mic);

}

function draw() {
	background(255);
	// rotateY(frameCount*.004);
	// if (frameCount % 360 === 60) rootNode.stimulate();
	// rootNode.draw();


	let spectrum = fft.analyze();
	// console.log(spectrum)
	textSize(32);
	text('word', 10, 30);
	fill(0, 102, 153);
	text(JSON.stringify(spectrum), 10, 60);
	fill(0, 102, 153, 51);
	text('word', 10, 90);
	beginShape();
  for (i = 0; i < spectrum.length; i++) {
    vertex(i, map(spectrum[i], 0, 255, height, 0));
  }
  endShape();
}

function touchStarted() {
  if (getAudioContext().state !== 'running') {
    getAudioContext().resume();
	}
}