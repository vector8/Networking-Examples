using UnityEngine;
using System.Collections.Generic;

public class BoidFlock : MonoBehaviour
{
    public int numberOfBoids;
    public GameObject boidPrefab;

    public float pullFactor, proximityFactor, inertiaFactor;
    public float neighbourDistance;

    public float maxVelocity;
    public Vector3 limits;

    public List<Boid> flock;

    const float CORRECTION_ACCELERATION = 2f;

    // Use this for initialization
    void Start()
    {
        flock = new List<Boid>();
        for(int i = 0; i < numberOfBoids; i++)
        {
            GameObject go = GameObject.Instantiate<GameObject>(boidPrefab);
            go.transform.position = new Vector3(Random.Range(-1f, 1f), Random.Range(-1f, 1f), Random.Range(-1f, 1f));
            go.transform.SetParent(transform);
            Boid b = go.GetComponent<Boid>();
            b.id = i;
            flock.Add(b);
        }
    }

    // Update is called once per frame
    void Update()
    {
        Vector3 centre = new Vector3(), inertia = new Vector3();
        Vector3 alignment = new Vector3();
        foreach(Boid b in flock)
        {
            centre += b.transform.position;
            inertia += b.velocity;
        }

        centre /= numberOfBoids;
        inertia /= numberOfBoids;
        alignment = inertia * inertiaFactor;

        foreach(Boid b in flock)
        {
            Vector3 pull = new Vector3(), separation = new Vector3();

            Vector3 localCentre = new Vector3();
            localCentre += b.transform.position;
            int numNeighbours = 1;
            foreach (Boid b2 in flock)
            {
                if(b2.id != b.id && (Vector3.SqrMagnitude(b2.transform.position - b.transform.position) < neighbourDistance))
                {
                    localCentre += b2.transform.position;
                    numNeighbours++;
                }
            }
            localCentre /= numNeighbours;

            pull = (centre - b.transform.position) * pullFactor;
            separation = (b.transform.position - localCentre) * proximityFactor;
            Vector3 acceleration = alignment + pull + separation;

            // Screen limits
            if(b.transform.position.x < -limits.x)
            {
                acceleration.x = CORRECTION_ACCELERATION;
            }
            else if(b.transform.position.x > limits.x)
            {
                acceleration.x = -CORRECTION_ACCELERATION;
            }
            if (b.transform.position.y < -limits.y)
            {
                acceleration.y = CORRECTION_ACCELERATION;
            }
            else if (b.transform.position.y > limits.y)
            {
                acceleration.y = -CORRECTION_ACCELERATION;
            }
            if (b.transform.position.z < -limits.z)
            {
                acceleration.z = CORRECTION_ACCELERATION;
            }
            else if (b.transform.position.z > limits.z)
            {
                acceleration.z = -CORRECTION_ACCELERATION;
            }

            b.velocity += acceleration * Time.deltaTime;

            b.velocity = Vector3.ClampMagnitude(b.velocity, maxVelocity);

            b.transform.position += b.velocity * Time.deltaTime;

            // turn the boid toward their velocity
            b.transform.LookAt(b.transform.position + Vector3.Normalize(b.velocity));
        }

    }
}
