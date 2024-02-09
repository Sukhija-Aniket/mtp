#include "ns3/simulator.h"
#include "custom-mobility-model.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CustomMobilityModel);

TypeId CustomMobilityModel::GetTypeId (void) 
{
  static TypeId tid = TypeId ("ns3::CustomMobilityModel")
	.SetParent<MobilityModel> ()
	.SetGroupName ("Mobility")
	.AddConstructor<CustomMobilityModel> ();
	return tid;
}
TypeId CustomMobilityModel::GetInstanceTypeId() const
{
	return CustomMobilityModel::GetTypeId();
}
CustomMobilityModel::CustomMobilityModel() {
	m_theta =0 ; //heading is initialized to zero
	m_min_velocity = 0; //Cannot go backward. Minimum speed is stand still.
	m_max_velocity = 30;
	m_min_acceleration = -3.6576; // m/s^2
	m_max_acceleration = 3.6576;
}
CustomMobilityModel::~CustomMobilityModel() {
}

void CustomMobilityModel::SetAccelerationValue (double value)
{
	double newValue;
	if (value>m_max_acceleration)
	{
		newValue = m_max_acceleration;
	}
	if (value<m_min_acceleration)
	{
		newValue = m_min_acceleration;
	}
	else
	{
		newValue = value;
	}

	Vector newAccerl (newValue, 0,0);
	SetVelocityAndAcceleration (GetVelocity(), newAccerl);
}


inline Vector
CustomMobilityModel::DoGetVelocity (void) const
{
	double t = (Simulator::Now () - m_baseTime).GetSeconds ();

	return Vector (m_baseVelocity.x + m_acceleration.x*t,
			m_baseVelocity.y + m_acceleration.y*t,
			m_baseVelocity.z + m_acceleration.z*t);
}

inline Vector
CustomMobilityModel::DoGetPosition (void) const
{
	double t = (Simulator::Now () - m_baseTime).GetSeconds ();

	double half_t_square = t*t*0.5;

	return Vector (m_basePosition.x + m_baseVelocity.x*t + m_acceleration.x*half_t_square,
			m_basePosition.y + m_baseVelocity.y*t + m_acceleration.y*half_t_square,
			m_basePosition.z + m_baseVelocity.z*t + m_acceleration.z*half_t_square);
}

void
CustomMobilityModel::DoSetPosition (const Vector &position)
{
	m_baseVelocity = DoGetVelocity ();
	m_baseTime = Simulator::Now ();
	m_basePosition = position;
	NotifyCourseChange ();
}

void CustomMobilityModel::SetVelocityAndAcceleration(const Vector &velocity, const Vector &acceleration)
{
	m_basePosition = DoGetPosition ();
	m_baseTime = Simulator::Now ();
	m_baseVelocity = velocity;
  	m_acceleration = acceleration;

	//Max sure the acceleration input doesn't exceed the limits.
	if (m_acceleration.x > m_max_acceleration)
		m_acceleration.x = m_max_acceleration;
	else if (m_acceleration.x < m_min_acceleration)
		m_acceleration.x = m_min_acceleration;


    //SanityCheck(); //Used this for debugging.

    NotifyCourseChange ();
		//Check if we're going to violate speed limits (0 to m_max_velocity)
    if (acceleration.x > 0 && m_baseVelocity.x == m_max_velocity) //trying to speedup when velocity is max
    {
		m_acceleration = Vector (0,0,0);
        return;
    }
    else if (acceleration.x < 0 && m_baseVelocity.x == 0) //Trying to slow down when velocity is zero
    {
        m_acceleration = Vector (0,0,0);
        return;
    }
    if (acceleration.x == 0)
    {
        return;
    }
    //Okay, so we are either "speedig up, but not at max speed" or "slowing down but not stand-still"
	/*
		A node maintains a certain acceleration until it hits top speed, when it hit top speed, acceleration is set to 0
		An event is created & sechudled to fire when max speed is reached to set the acceleration to 0.
		We're cancelling that event because acceleration has changed
	*/
	m_limit_event.Cancel();

	//If a node maintains this acceleration, when will it hit its speed limit?

    if (m_acceleration.x < 0) //we're slowing down, let's find the time it takes to reach 0
    {
        //Scheduling an event to set acceleration to zero
        m_timeToSpeedlimit = Seconds( m_baseVelocity.x / std::abs(m_acceleration.x) );
    }
    else if (m_acceleration.x > 0) //We're speeding up, let's find the time it takes to reach max velocity
    {
        m_timeToSpeedlimit = Seconds( (m_max_velocity - m_baseVelocity.x) / m_acceleration.x );
    }
	else
	{
		m_timeToSpeedlimit = Seconds(0); //for completion purposes
	}

	/*
	We will schedule an event for when will the velocity limit be reached
	*/
	//Let's check if the calculated time to reach the limit is not zero
	if (m_timeToSpeedlimit.GetSeconds() > 0 )
	{
			//If we're speeding up...
			if (m_acceleration.x >0)
					m_limit_event = Simulator::Schedule(m_timeToSpeedlimit, &CustomMobilityModel::SetAccelerationToZeroMaxVelocity, this);
			else
			{		//If we're slowing down...
					m_limit_event = Simulator::Schedule(m_timeToSpeedlimit, &CustomMobilityModel::SetAccelerationToZeroZeroVelocity, this);
			}

    }
	else	//If the calculated time to speed limit is zero, set acceleration to zero.
	{
		m_acceleration = Vector (0,0,0);
	}

	//NotifyCourseChange ();
}
void CustomMobilityModel::SetAccelerationToZeroZeroVelocity()
{
    SetVelocityAndAcceleration ( Vector(0,0,0) , Vector(0,0,0));
}
/*
Vector CustomMobilityModel::GetPosition (void) const
{
	return DoGetPosition ();
}
*/
void CustomMobilityModel::SetAccelerationToZeroMaxVelocity ()
{
    SetVelocityAndAcceleration ( Vector(m_max_velocity,0,0) , Vector(0,0,0));

}


Vector CustomMobilityModel::GetAcceleration() {
	return m_acceleration;
}

void CustomMobilityModel::SetAccelerationLimits(double min, double max) {
	m_min_acceleration = min;
	m_max_acceleration = max;
}

void CustomMobilityModel::SetMaxVelocity(double max) {
	this->m_max_velocity = max;
}

double CustomMobilityModel::GetAccelerationValue ()
{
	return m_acceleration.x;
}

void CustomMobilityModel::SanityCheck()
{
    //sanity check
	if (m_baseVelocity.x < 0)
    {
        // std::cout << RED ("SPEED IS NEGATIVE! : ") << m_baseVelocity.x << std::endl;
    }
    if (m_baseVelocity.x > m_max_velocity)
    {
        // std::cout << YELLOW ("SPEED LIMIT EXCEEDED : ") << m_baseVelocity.x << std::endl;
    }
}

} // namespace ns3
