/*
	Simbicon 1.5 Controller Editor Framework, 
	Copyright 2009 Stelian Coros, Philippe Beaudoin and Michiel van de Panne.
	All rights reserved. Web: www.cs.ubc.ca/~van/simbicon_cef

	This file is part of the Simbicon 1.5 Controller Editor Framework.

	Simbicon 1.5 Controller Editor Framework is free software: you can 
	redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Simbicon 1.5 Controller Editor Framework is distributed in the hope 
	that it will be useful, but WITHOUT ANY WARRANTY; without even the 
	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Simbicon 1.5 Controller Editor Framework. 
	If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <Physics/AbstractRBEngine.h>
#include <ode/ode.h>
//#include <ode/joint.h>
#include <Physics/CollisionDetectionPrimitive.h>
#include <Physics/SphereCDP.h>
#include <Physics/CapsuleCDP.h>
#include <Physics/BoxCDP.h>
#include <Physics/PlaneCDP.h>
#include <Physics/PreCollisionQuery.h>

#define MAX_CONTACT_FEEDBACK 200


//this structure is used to map a rigid body to the id of its ODE counterpart
typedef struct ODE_RB_Map_struct{
	dBodyID id;
	RigidBody* rb;
	struct ODE_RB_Map_struct(dBodyID newId, RigidBody* newRb){ this->id = newId; this->rb = newRb;}
} ODE_RB_Map;

/*-------------------------------------------------------------------------------------------------------------------------------------------------*
 * This class is used as a wrapper that is designed to work with the Open Dynamics Engine. It uses all the rigid bodies (together with the joints) *
 * that are loaded with RBCollection, and has methods that link with ODE to simulate the physics. If a different physics engine is to be used,     *
 * then ideally only the methods of this class need to be re-implemented, and the rest of the application can stay the same.                       *
 *-------------------------------------------------------------------------------------------------------------------------------------------------*/
class ODEWorld : public AbstractRBEngine{
friend void collisionCallBack(void* odeWorld, dGeomID o1, dGeomID o2);

private:
	// ODE's id for the simulation world
	dWorldID worldID;
	// id of collision detection space
	dSpaceID spaceID;
	// id of contact group
	dJointGroupID contactGroupID;
	//keep track of the mapping between the rigid bodies and their ODE counterparts with this
	DynamicArray<ODE_RB_Map> odeToRbs;

	//keep an array of contact points that is used for each pair of geom collisions
	dContact *cps;
	//this is the max number of contacts that are going to be processed between any two objects
	int maxContactCount;

	dJointFeedback jointFeedback[MAX_CONTACT_FEEDBACK];
	//this is the current number of contact joints, for the current step of the simulation
	int jointFeedbackCount;

	//this is a pointer to a physical interface object that is used as an abstract way of communicating between the simulator and the application
	PreCollisionQuery* pcQuery;

	/**
		This method is used to set up an ode hinge joint, based on the information in the hinge joint passed in as a parameter
	*/
	void setupODEHingeJoint(HingeJoint* hj);

	/**
		This method is used to set up an ode universal joint, based on the information in the universal joint passed in as a parameter
	*/
	void setupODEUniversalJoint(UniversalJoint* uj);

	/**
		This method is used to set up an ode ball-and-socket joint, based on the information in the ball in socket joint passed in as a parameter
	*/
	void setupODEBallAndSocketJoint(BallInSocketJoint* basj);

	/**
		this method is used to copy the state of the ith rigid body to its ode counterpart.
	*/
	void setODEStateFromRB(int i);

	/**
		this method is used to copy the state of the ith rigid body, from the ode object to its rigid body counterpart 
	*/
	void setRBStateFromODE(int i);

	/**
		this method is used to set up an ODE sphere geom. It is properly placed in body coordinates.
	*/
	dGeomID getSphereGeom(SphereCDP* s);

	/**
		this method is used to set up an ODE box geom. It is properly placed in body coordinates.
	*/
	dGeomID getBoxGeom(BoxCDP* b);

	/**
		this method is used to set up an ODE plane geom. It is properly placed in body coordinates.
	*/
	dGeomID getPlaneGeom(PlaneCDP* p, RigidBody* parent);

	/**
		this method is used to set up an ODE sphere geom. It is properly placed in body coordinates.
	*/
	dGeomID getCapsuleGeom(CapsuleCDP* c);

	/**
		this method is used to process the collision between the two objects passed in as parameters. More generally,
		it is used to determine if the collision should take place, and if so, it calls the method that generates the
		contact points.
	*/
	void processCollisions(dGeomID o1, dGeomID o2);

	/**
		this method is used to create ODE geoms for all the collision primitives of the rigid body that is passed in as a paramter
	*/
	void createODECollisionPrimitives(RigidBody* body);

	/**
		this method is used to transfer the state of the rigid bodies, from the simulator to the rigid body wrapper
	*/
	virtual void setRBStateFromEngine();

	/**
		this method is used to transfer the state of the rigid bodies, from the rigid body wrapper to the simulator's rigid bodies
	*/
	virtual void setEngineStateFromRB();

public:
	/**
		default constructor
	*/
	ODEWorld();

	/**
		destructor
	*/
	virtual ~ODEWorld(void);

	/**
		This method reads a list of rigid bodies from the specified file.
	*/
	virtual void loadRBsFromFile(char* fName);

	/**
		This method is used to integrate the forward simulation in time.
	*/
	virtual void advanceInTime(double deltaT);

	/**
		this method applies a force to a rigid body, at the specified point. The point is specified in local coordinates,
		and the force is also specified in local coordinates.
	*/
	virtual void applyRelForceTo(RigidBody* b, const Vector3d& f, const Point3d& p);

	/**
		This method is used to set the state of all the rigid body in the physical world.
	*/
	void setState(DynamicArray<double>* state, int start = 0);

	/**
		this method applies a force to a rigid body, at the specified point. The point is specified in local coordinates,
		and the force is specified in world coordinates.
	*/
	virtual void applyForceTo(RigidBody* b, const Vector3d& f, const Point3d& p);

	/**
		this method applies a torque to a rigid body. The torque is specified in world coordinates.
	*/
	virtual void applyTorqueTo(RigidBody* b, const Vector3d& t);

	/**
		This method is used to integrate the forward simulation in time.
	*/
	void testAdvanceInTime(double deltaT);

	/**
		run a testing method...
	*/
	virtual void runTest();

};
