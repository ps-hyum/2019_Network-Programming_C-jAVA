package main;
import java.util.HashMap;

public class LoginUserMap {
	private HashMap<String, Integer> map;
	
	public LoginUserMap() {
		map = new HashMap<String, Integer>();
	}
	
	public boolean checkUsing(String id) {
		return map.containsKey(id);
	}
	
	public void updateUsing(String id) {
		map.put(id, 0);
	}
	
	public void deleteUsing(String id) {
		map.remove(id);
	}
}
