#include "script.h"

//TODO: Tinker with FOV and camera position

Cam putCameraOnVehicle(Cam camera, Vehicle vehicle) {
	if (!CAM::DOES_CAM_EXIST(camera)) {
		if (ENTITY::DOES_ENTITY_EXIST(vehicle)) {
			Vector3 rotation = ENTITY::GET_ENTITY_ROTATION(vehicle, 1);

			CAM::DESTROY_ALL_CAMS(TRUE);
			camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", TRUE);
			CAM::SET_CAM_FOV(camera, 50);
			CAM::SET_CAM_INHERIT_ROLL_VEHICLE(camera, TRUE);
			CAM::SET_CAM_ACTIVE(camera, TRUE);			

			CAM::ATTACH_CAM_TO_ENTITY(camera, vehicle, 0, 2.0, 0.4, TRUE);
			CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);
			CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);
		}
	}
	else {
		if (ENTITY::DOES_ENTITY_EXIST(vehicle)) {
			Vector3 rotation = ENTITY::GET_ENTITY_ROTATION(vehicle, 1);
			CAM::ATTACH_CAM_TO_ENTITY(camera, vehicle, 0, 2.0, 0.4, TRUE);
			CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);
			CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);
		}
		else {
			CAM::RENDER_SCRIPT_CAMS(FALSE, FALSE, 0, TRUE, TRUE);
			CAM::SET_CAM_ACTIVE(camera, FALSE);
			CAM::DESTROY_CAM(camera, TRUE);
			camera = NULL;
		}
		
	}	
	
	return camera;

}