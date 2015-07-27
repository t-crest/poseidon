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
	public final int x, y;
	private static int width, height;
	public TileCoord(int x, int y, int width, int height){
		if((x >= width || y >= height) || (x < 0 || y < 0)){
			throw new IndexOutOfBoundsException("Tile coordinate out of network.");
		}
		this.x = x;
		this.y = y;
		this.width = width;
		this.height = height;
	}
	public TileCoord(int x, int y){
		if((x >= this.width || y >= this.height) || (x < 0 || y < 0)){
			throw new IndexOutOfBoundsException("Tile coordinate out of network.");
		}
		this.x = x;
		this.y = y;
	}
	public int getTileId(){
		return x+y*width;
	}

	public TileCoord moveNorth() {
		if(y == 0){
			return new TileCoord(x, height-1);
		} else {
			return new TileCoord(x, y-1);
		}
	}

	public TileCoord moveSouth(){
		if(y == height-1){
			return new TileCoord(x, 0);
		} else {
			return new TileCoord(x, y+1);
		}
	}
	public TileCoord moveEast(){
		if(x == width-1){
			return new TileCoord(0, y);
		} else {
			return new TileCoord(x+1, y);
		}
	}
	public TileCoord moveWest(){
		if(x == 0){
			return new TileCoord(width-1, y);
		} else {
			return new TileCoord(x-1, y);
		}
	}

	@Override
	public String toString(){
		return "(" + x + "," +  y + ")";
	}

	public int getNumOfNodes(){
		return width*height;
	}
	
	public int getWidth(){
		return width;
	}
	
	public int getHeight(){
		return height;
	}
}
