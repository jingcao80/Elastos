using UnityEngine;
using System.Collections;
 
public class mouseControl : MonoBehaviour {
	     
	     
    private float x = 0.0f;
    private float y = 0.0f;
     
    private float xSpeed = 200.0f;
    private float ySpeed = 200.0f;
     
    private float zSpeed = 40f;
     
    private float normalDistence = 0;
     
    private float minDistence = 0;
    private float maxDistence = 10;
     
 
    void Update ()
	{
		         
		if (Input.GetMouseButton (0)) {
			x = Input.GetAxis ("Mouse X") * xSpeed;
			//print(Input.GetAxis("Mouse X"));
			y = Input.GetAxis ("Mouse Y") * ySpeed;
			             
			transform.Rotate (Vector3.up * -x * Time.deltaTime, Space.World);
			transform.Rotate (Vector3.right * y * Time.deltaTime, Space.World);
			             
		} else if (Input.GetAxis ("Mouse ScrollWheel") != 0) {
			float ga = Input.GetAxis ("Mouse ScrollWheel");
			//print ("ga:" + ga);
			//print ("z:" + transform.position.z);
			if (transform.position.z >= minDistence && transform.position.z <= maxDistence) {
				transform.Translate (Vector3.forward * -Input.GetAxis ("Mouse ScrollWheel") * zSpeed * Time.deltaTime, Space.World); 
			} else {
				Vector3 tmp = transform.position;

				if (transform.position.z < minDistence) {
					tmp.z = minDistence;
					transform.position = tmp;
				} else {
					tmp.z = maxDistence;
					transform.position = tmp;
				}
			}
		}
	}
}