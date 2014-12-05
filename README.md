BitCoinSensor
=============

An IoT solution for monitoring the price of Bitcoin with Visual and Auditory cues pertaining to the volatility of the the CoinDesk exchange over a given time interval.     

This project obtains the latest BTC price from CoinDesk every 30s and displays the value with a Green or Red background on the LCD based on whether the price has increased or decreased respectively since the last request.

Volatility is a measure of how much the price of a financial asset varies over time. This project also allows you to set an auditory alarm which sounds when the volatility index is exceeded. 

The Volatility Index is computed using the exponential moving average formula:

accumulator = (alpha * currentNumericRate) + (1.0 - alpha) * accumulator

volatilityIndex = (currentNumericRate / accumulator) - 1

Note that the volatilityIndex is directional, thus if it is negative the volatility is moving the price downward and vice versa if positive.  


The idea behind this project is to alert a potential investor of breakneck swings in the price of BTC.  This could be very important given that the BTC exchange is open 24/7.  Thus, a savvy investor could potentially save themselves from a heavy loss if they were alerted to say a 5% negative volatilityIndex swing that were to occur while the investor is sleeping.  Likewise, one could be alerted of a peak selling opportunity should the volatilityIndex report a rise.  

Schematic
========
![ScreeenShot](http://i.imgur.com/IEt8H17.png)

In-Action
=========
![ScreenShot](http://giant.gfycat.com/BlondExemplaryKoala.gif)

