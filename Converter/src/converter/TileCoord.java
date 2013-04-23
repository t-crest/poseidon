
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
