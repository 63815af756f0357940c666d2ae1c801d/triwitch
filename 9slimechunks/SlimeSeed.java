package tmptest;
import java.util.Random;

public class SlimeSeed {
	static final long seed=-229556285580460297L;
	static int hit(int xPosition, int zPosition){
		Random rnd = new Random(seed +
                (long) (xPosition * xPosition * 0x4c1906) +
                (long) (xPosition * 0x5ac0db) +
                (long) (zPosition * zPosition) * 0x4307a7L +
                (long) (zPosition * 0x5f24f) ^ 0x3ad8025f);
			if(rnd.nextInt(10) == 0)return 1;
			else return 0;
	}
	static int hit9(int x,int z){
		return hit(x - 1, z - 1) + hit(x - 1, z) + hit(x - 1, z + 1) + hit(x, z - 1) + hit(x, z) + hit(x, z + 1) + hit(x + 1, z - 1) + hit(x + 1, z) + hit(x + 1, z + 1);
	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		int x=0;
		int z=0;
		int maxhit=0;
		while(true){
			int hitval = hit9(x, z);
			if ((hitval > maxhit)||(hitval>=9)){				
				System.out.println(String.valueOf(hitval)+" "+String.valueOf(x)+" "+String.valueOf(z));
				maxhit=hitval;
			}
			if ((z > 0)&&((-x < z) && (x <= z))) {
				x--;
			}
			else if ((z <= 0) && ((-x <= -z) && (x <= -z))) {
				x++;
			}
			else if ((x > 0) && ((-z < x) && (z < x))) {
				z++;
			}
			else if ((x < 0) && ((-z < -x) && (z <= -x))) {
				z--;
			}
		}
	}

}
