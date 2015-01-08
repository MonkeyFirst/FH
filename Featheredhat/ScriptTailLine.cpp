#include "Common.h"
#include "ScriptTailLine.h"

ScriptTailLine::ScriptTailLine(Context* context) : LogicComponent(context)
{
	SetUpdateEventMask(USE_FIXEDUPDATE | USE_UPDATE);
}

void ScriptTailLine::SetupTailLine(Vector3 start, Vector3 end, float lifeTime, float width, TailMaterial tm )
{
	start_ = start;
	end_ = end;
	lifeTime_ = lifeTime;

	const unsigned numVertices = 12;

	PODVector<DYN_VERTEX> vertexData;
	vertexData.Resize(numVertices);
	PODVector<unsigned short> indexData;
	indexData.Resize(numVertices);

	Vector3 rightDelta = Vector3(width, 0.0f, 0.0f);
	Vector3 upDelta = Vector3(0.0f, width, 0.0f);

	/*
	4----51
	|   / |
	| /   |
	32----0
	*/

	// plane 1
	vertexData[0].p = start_ - rightDelta;
	vertexData[1].p = end_ - rightDelta;
	vertexData[2].p = start_ + rightDelta;
	vertexData[3].p = start_ + rightDelta;
	vertexData[4].p = end_ + rightDelta;
	vertexData[5].p = end_ - rightDelta;

	vertexData[0].n = Vector3::UP;
	vertexData[1].n = Vector3::UP;
	vertexData[2].n = Vector3::UP;
	vertexData[3].n = Vector3::UP;
	vertexData[4].n = Vector3::UP;
	vertexData[5].n = Vector3::UP;

	vertexData[0].uv = Vector2(1, 0);
	vertexData[1].uv = Vector2(1, 1);
	vertexData[2].uv = Vector2(0, 0);
	vertexData[3].uv = Vector2(0, 0);
	vertexData[4].uv = Vector2(0, 1);
	vertexData[5].uv = Vector2(1, 1);


	//// plane 2
	vertexData[6].p = start_ - upDelta;
	vertexData[7].p = end_ - upDelta;
	vertexData[8].p = start_ + upDelta;
	vertexData[9].p = start_ + upDelta;
	vertexData[10].p = end_ + upDelta;
	vertexData[11].p = end_ - upDelta;

	vertexData[6].n = Vector3::RIGHT;
	vertexData[7].n = Vector3::RIGHT;
	vertexData[8].n = Vector3::RIGHT;
	vertexData[9].n = Vector3::RIGHT;
	vertexData[10].n = Vector3::RIGHT;
	vertexData[11].n = Vector3::RIGHT;

	vertexData[6].uv = Vector2(1, 0);
	vertexData[7].uv = Vector2(1, 1);
	vertexData[8].uv = Vector2(0, 0);
	vertexData[9].uv = Vector2(0, 0);
	vertexData[10].uv = Vector2(0, 1);
	vertexData[11].uv = Vector2(1, 1);



	for (int i = 0; i < numVertices; ++i)
	{
		indexData[i] = i;
	}

	vb->SetSize(numVertices, MASK_POSITION | MASK_NORMAL | MASK_TEXCOORD1);
	vb->SetData(&vertexData[0]);


	ib->SetShadowed(true);
	ib->SetSize(numVertices, false);
	ib->SetData(&indexData[0]);

	geom->SetVertexBuffer(0, vb);
	geom->SetIndexBuffer(ib);
	geom->SetDrawRange(TRIANGLE_STRIP, 0, numVertices);



	fromScratchModel->SetNumGeometries(1);
	fromScratchModel->SetGeometry(0, 0, geom);
	fromScratchModel->SetBoundingBox(BoundingBox(Sphere(start_, (start_ - end_).Length())));
	

	StaticModel* tailModel = GetNode()->CreateComponent<StaticModel>();
	tailModel->SetModel(fromScratchModel);

	
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	switch (tm)
	{
	case ScriptTailLine::WHITE:
		tailModel->SetMaterial(cache->GetResource<Material>("Materials/TailLine.xml"));
		break;
	case ScriptTailLine::RED:
		tailModel->SetMaterial(cache->GetResource<Material>("Materials/TailLineRed.xml"));
		break;
	default:
		break;

	}




}

void ScriptTailLine::RegisterObject(Context* context) 
{
	context->RegisterFactory<ScriptTailLine>();
}

void ScriptTailLine::Start() 
{
	fromScratchModel = new Model(context_);
	vb = new VertexBuffer(context_);
	ib = new IndexBuffer(context_);
	geom = new Geometry(context_);

	currentLifeTime_ = 0;

}

void ScriptTailLine::Update(float timeStep) 
{
	if (lifeTime_ < currentLifeTime_)
	{
		GetNode()->Remove();
		
	}
	else
		currentLifeTime_ += timeStep;

}

void ScriptTailLine::FixedUpdate(float timeStep) 
{

	
	//GetNode()->RemoveComponent<ScriptTailLine>();
}