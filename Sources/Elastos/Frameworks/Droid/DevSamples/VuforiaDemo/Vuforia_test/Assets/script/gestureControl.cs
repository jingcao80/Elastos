using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class gestureControl : MonoBehaviour {

	//放系数  
	private float distance = 0.1f;  

	private float x = 0.0f;
	private float y = 0.0f;

	private float xSpeed = 200.0f;
	private float ySpeed = 200.0f;
	private float zSpeed = 40f;
	 

	// 上一次手机触摸位置判断用 是在做放大 小手 
	private Vector2 oldPosition1 = new Vector2 (0, 0);  
	private Vector2 oldPosition2 = new Vector2 (0, 0); 

	private float minDistence;
	private float maxDistence;

	//初始化游信息置  
	void Start ()  
	{  
		Debug.Log (oldPosition1); 
		minDistence = transform.position.z - 10;
		maxDistence = transform.position.z + 10;
	}  

	void Update ()  
	{  
		// 判断触摸数量点触摸  
		if (Input.touchCount == 1) {  
			// 触摸型移触摸  
			if (Input.GetTouch (0).phase == TouchPhase.Moved) {  
				//根据触摸点算X与Y位置  
				x = Input.GetAxis ("Mouse X") * xSpeed * 0.2f;  
				y = Input.GetAxis ("Mouse Y") * ySpeed * 0.2f;

				transform.Rotate (Vector3.up * -x * Time.deltaTime, Space.World);
				transform.Rotate (Vector3.right * -y * Time.deltaTime, Space.World);
			}  
		}  

		// 判断触摸数量多点触摸  
		if (Input.touchCount > 1) {  
			// 前两只手指触摸型都移触摸  
			if (Input.GetTouch (0).phase == TouchPhase.Moved || Input.GetTouch (1).phase == TouchPhase.Moved) {  
				// 算出当前两点触摸点的位置  
				var tempPosition1 = Input.GetTouch (0).position;  
				var tempPosition2 = Input.GetTouch (1).position;  
				// 函数返回真放大，返回假小  
				if (isEnlarge (oldPosition1, oldPosition2, tempPosition1, tempPosition2)) {  
					// 放大系数超3以后不允放大  
					// 里的数据是根据我目中的模型而的，大家可以自己任意修改  
					if (transform.position.z < maxDistence) {
						transform.Translate (Vector3.forward * distance * zSpeed * Time.deltaTime, Space.World);
					}					 
				} else {  
					// 小系数返回18.5后不允小  
					// 里的数据是根据我目中的模型而的，大家可以自己任意修改  
					if (transform.position.z > minDistence) {
						transform.Translate (Vector3.forward * -distance * zSpeed * Time.deltaTime, Space.World);
					}  
				}  
				// 份上一次触摸点的位置，用于比  
				oldPosition1 = tempPosition1;  
				oldPosition2 = tempPosition2;  

			}  
		}  
	}  

	// 函数返回真为放大，返回假为缩小  
	bool isEnlarge (Vector2 oP1, Vector2 oP2, Vector2 nP1, Vector2 nP2)  
	{  
		// 函数传入上一次触摸两点的位置与本次触摸两点的位置计算出用户的手势  
		float leng1 = Mathf.Sqrt ((oP1.x - oP2.x) * (oP1.x - oP2.x) + (oP1.y - oP2.y) * (oP1.y - oP2.y));  
		float leng2 = Mathf.Sqrt ((nP1.x - nP2.x) * (nP1.x - nP2.x) + (nP1.y - nP2.y) * (nP1.y - nP2.y));  

		if (leng1 < leng2) {  
			// 放大手势  
			return true;  
		} else {  
			// 缩小手势  
			return false;  
		}  
	}  

	// Update方法一旦 
	void LateUpdate ()  
	{  

		// target主角，放旋的参照物  
		if (false) {         
			// 重置像机的位置  
			//y = ClampAngle (y, yMinLimit, yMaxLimit);  
			//Quaternion rotation = Quaternion.Euler (y, x, 0);  
			//Vector3 position = rotation * new Vector3 (0.0f, 0.0f, -distance) + target.position;  

			//transform.rotation = rotation;  
			//transform.position = position;  
		}  
	}  

	static float ClampAngle (float  angle, float min, float max)  
	{  
		if (angle < -360)  
			angle += 360;  
		if (angle > 360)  
			angle -= 360;  
		return Mathf.Clamp (angle, min, max);  
	}  
}
