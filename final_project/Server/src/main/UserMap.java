package main;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class UserMap {
	private HashMap<String, User> map;
	
	public UserMap() {
		map = new HashMap<String, User>();
	}
	
	public User getUser(String id) {
		return map.get(id);
	}
	
	public boolean checkAvlID(String id) {
		return map.containsKey(id);
	}
	
	public void updateUser(String name, String id, String password, int Pid) {
		User user = new User(name, id, password, Pid);
		map.put(id, user);
	}
	
	public void deleteUser(String id) {
		map.remove(id);
	}
	
	public String findID(String name, int Pid) {
		for(Map.Entry<String, User> elem : map.entrySet()) {
			User nowuser = elem.getValue();
			if(nowuser.getPid() == Pid) {
				if(nowuser.getName().equals(name))
					return nowuser.getId();
			}
		}
		return null;
	}
	
	public String findPWD(String id, int Pid) {
		User nowuser = map.get(id);
		if(nowuser.getPid() == Pid)
			return nowuser.getPassword();
		else
			return null;
	}
}
