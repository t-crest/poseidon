/*******************************************************************************
 * Copyright 2012 Rasmus Bo Soerensen <rasmus@rbscloud.dk>
 * Copyright 2013 Technical University of Denmark, DTU Compute.
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the
 * disclaimer below) provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the copyright holder.
 ******************************************************************************/
 
package converter;

/**
 *
 * @author Rasmus
 */
public class TileCoord {
	public int x, y;
	private static int sideLength;
	public TileCoord(int x, int y, int sideLength){
		this.x = x;
		this.y = y;
		this.sideLength = sideLength;
	}
	public TileCoord(int x, int y){
		this.x = x;
		this.y = y;
	}
	public int getTileId(){
		return x+y*sideLength;
	}
	
	public void moveNorth(){
		if(this.y == 0){
			this.y = sideLength-1; 
		} else {
			this.y--;
		}
	}
	
	public void moveSouth(){
		if(this.y == sideLength-1){
			this.y = 0; 
		} else {
			this.y++;
		}
	}
	public void moveEast(){
		if(this.x == sideLength-1){
			this.x = 0; 
		} else {
			this.x++;
		}
	}
	public void moveWest(){
		if(this.x == 0){
			this.x = sideLength-1; 
		} else {
			this.x--;
		}
	}
	
	@Override
	public String toString(){
		return "(" + x + "," +  y + ")";
	}
}
