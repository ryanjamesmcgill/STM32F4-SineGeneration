STM32F4-SineGeneration
======================

Demo project to create audio on an armStrap development board based the STM32F4 processor.
The armStrap creates a 200hz SINE wave at startup, then sends that data to external DAC via I2S serial
protocol in a continuous loop. 

<p> armStrap -> I2S3 port -> DAC -> audio output </p>

<p>
armStrap A4 - I2S3_WS <br>
armStrap C7 - I2S3_MCK <br>
armStrap C10 - I2S3_SCK <br>
armStrap C12 - I2S3_SD <br>
</p>

<p>The armStrap is a nice alternative to the STM32F4 Discovery board because it allows you develop on tools that are all free and available for Linux, Windows, and OSX. More info: (http://armstrap.org/)</p>

<p>For the DAC, I used a board based on the Cirrus Logic CS4334 made by Sure electronics. But you should be able
to use any board/chip that accepts I2S input, and audio output!
(http://www.amazon.com/Sure-Electronics-CS4334-stereo-amplifier/dp/B00KMT4NE8) </p>

<p><center>
<img src="https://cloud.githubusercontent.com/assets/6580936/5514052/d41bdf5c-87e3-11e4-8300-76a90fcae725.JPG" alt="Prototype Demo">
<img src="https://cloud.githubusercontent.com/assets/6580936/5514053/d8996720-87e3-11e4-93ac-14b0fa5991ed.jpg" alt="Prototype Layout">
</center></p>

