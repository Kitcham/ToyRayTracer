#version 330 core
//数据结构与常量-----------------------------------------
float t_max=10000000000000000.0;
float PI=3.1415926;
struct Ray{
	vec3 origin,direction;
	float R;
};
// 光线求交结果
struct HitResult {
    bool isHit;             // 是否命中
    bool frontFace;          // 视线射入面
    float distance1;         // 与交点的距离,ray 中的t
    vec3 hitPoint;          // 光线命中点
    vec3 normal;            // 命中点法线
    vec3 viewDir;           // 击中该点的光线的方向
	int Matrial;			//该点材质
	vec3 Color;
};
//ONB
struct ONB{
	vec3 axis[3];
};
//漫反射结果
struct scatterRecord{
	Ray specularRay;
	bool isSpecular;
	vec3 attention;
	int pdf;//cosinPDF=1 spherePDF=2 none=0
	ONB onb;
};
struct tracer{
	Ray ray;
	int deep;
};
//三角形
#define SIZE_TRIANGLE   5
struct Triangle {
	vec3 Matrial;
	vec3 v1, v2, v3;
	vec3 Color;
};
struct aabb{
	vec3 minimum, maximum;
};
//BVH节点
#define SIZE_BVHNODE 8
struct Node {
	aabb AABB;
	Triangle Tri;
	int TriFlag;//0=NULL 1=Triangle 2=aabb
};
struct xozLight{
	float x1,x0,z1,z0,y;
};
//全局变量-----------------------------------------------

//输入输出---------------------------------------------------
in vec3 pix;
in vec2 texcoords;
uniform int frameCounter;
uniform int width;
uniform int height;
uniform int numTri;//三角形数量
uniform xozLight Light;//发光平面
uniform vec3 eye;//摄像机点
//uniform samplerBuffer TriangleList;//纹理Buffer,三角形集合
uniform samplerBuffer BVHList;//BVH数组集合
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D gColor;
uniform sampler2D LastColor;
layout(location=0)out vec4  FragColor;
//解码输入----------------------------------------------
//Triangle getTriangle(int i) {//获取三角形集合
//    int offset = i * SIZE_TRIANGLE ;
//    Triangle t;
//    t.Matrial=texelFetch(TriangleList,offset+0).xyz;
//    // 顶点坐标
//    t.v1 = texelFetch(TriangleList, offset + 1).xyz;
//    t.v2 = texelFetch(TriangleList, offset + 2).xyz;
//    t.v3 = texelFetch(TriangleList, offset + 3).xyz;
//    // 颜色
//    t.Color = texelFetch(TriangleList, offset + 4).xyz;
//    return t;
//}
Node getBVH(int i) {//获取BVH
	int offset = i*SIZE_BVHNODE;
	Node node;
	node.AABB.minimum = texelFetch(BVHList,offset).xyz;
	node.AABB.maximum = texelFetch(BVHList,offset+1).xyz;
	node.Tri.Matrial = texelFetch(BVHList,offset+2).xyz;
	node.Tri.v1 = texelFetch(BVHList,offset+3).xyz;
	node.Tri.v2 = texelFetch(BVHList,offset+4).xyz;
	node.Tri.v3 = texelFetch(BVHList,offset+5).xyz;
	node.Tri.Color = texelFetch(BVHList,offset+6).xyz;
	node.TriFlag = int(texelFetch(BVHList,offset+7).x);
	return node;
}
//随机函数----------------------------------------------
uint seed = uint(
    uint((pix.x * 0.5 + 0.5) * width)  * uint(1973) + 
    uint((pix.y * 0.5 + 0.5) * height) * uint(9277) + 
    uint(frameCounter) * uint(26699)) | uint(1);

uint wang_hash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}
float rand() {
    return float(wang_hash(seed)) / 4294967296.0;
}
 float random_double(float minn, float maxx) {
    return minn + (maxx - minn) * rand();
}
//工具函数------------------------------------------------------
// 半球均匀采样
vec3 SampleHemisphere() {
    float z = rand();
    float r = max(0, sqrt(1.0 - z*z));
    float phi = 2.0 * PI * rand();
    return vec3(r * cos(phi), r * sin(phi), z);
}
// 将向量 v 投影到 N 的法向半球
vec3 toNormalHemisphere(vec3 v, vec3 N) {
    vec3 helper = vec3(1, 0, 0);
    if(abs(N.x)>0.999) helper = vec3(0, 0, 1);
    vec3 tangent = normalize(cross(N, helper));
    vec3 bitangent = normalize(cross(N, tangent));
    return v.x * tangent + v.y * bitangent + v.z * N;
}
bool nearZero(vec3 v) {
	float a=1e-8;
	if(abs(v.x)<=a&&abs(v.y)<=a&&abs(v.z)<=a) return true;
	else return false;
}
//ONB build_from_w
void build_from_w(vec3 n, inout ONB onb) {
	onb.axis[2] = normalize(n);
	vec3 a = ((abs(onb.axis[2].x)>0.9) ? vec3(0,1,0) : vec3(1,0,0));
	onb.axis[1] = normalize(cross(onb.axis[2],a));
	onb.axis[0] = cross(onb.axis[2],onb.axis[1]);
}
vec3 random_cosine_direction() {
    float r1 = rand();
    float r2 = rand();
    float z = sqrt(1 - r2);

    float phi = 2 * PI * r1;
    float x = cos(phi) * sqrt(r2);
    float y = sin(phi) * sqrt(r2);

    return vec3(x, y, z);
}
// 极坐标形式
vec3 random_unit_vector() {
    float a = random_double(0, 2 * PI);
    float z = random_double(-1, 1);
    float r = sqrt(1 - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}
//计算模块------------------------------------------------------
//Möller-Trumbore 射线三角相交算法
bool TriangleHit(Triangle tri,Ray ray,out HitResult rec) {

	bool hitResult;
	hitResult=false;
	vec3 E1,E2,S,S1,S2,n;
	E1=tri.v2-tri.v1;
	E2=tri.v3-tri.v1;
	S=ray.origin-tri.v1;
	S1=cross(ray.direction,E2);
	S2=cross(S,E1);
	n=normalize(cross(E1,E2));//法向量

	float ES=dot(E1,S1);
	if(abs(ES)<(1e-5)){
		return false;
	}
	if(dot(n,ray.direction)>0.0f){
		n=-n;
		rec.frontFace=false;
	}else rec.frontFace=true;

	float f=1.0/ES;
	float t,b1,b2;
	//ES=dot(E1,S1);
	t=dot(E2,S2)*f;
	if(t<0||t<0.01) return false;
	b1=dot(S1,S)*f;
	if(b1<0||b1>1) return false;
	b2=dot(S2,ray.direction)*f;
	if(b2<0||b1+b2>1) return false;
	if(t>1e-5){
		hitResult=true;

		rec.isHit=true;
		rec.distance1=t;
		rec.hitPoint=ray.origin+ray.direction*t;
		rec.normal=n;
		rec.Matrial=int(tri.Matrial.x);
		rec.Color=tri.Color;

	}
	return hitResult;
}
bool AABBHit(aabb AABB,Ray ray,float t_min,float t_max) {
	for(int i=0;i<3;i++){
		float invD = 1.0f/ray.direction[i];
		float t0 = (AABB.minimum[i]-ray.origin[i])*invD;
		float t1 = (AABB.maximum[i]-ray.origin[i])*invD;
		if(invD<0.0f){
			float t3=t0;
			t0=t1;
			t1=t3;
		}
		t_min = t0 > t_min ? t0 : t_min;
		t_max = t1 < t_max ? t1 : t_max;
		if(t_max<=t_min){
			return false;
		}
 	}
	return true;
}
float hitAABB(Ray r, aabb AABB) {
    vec3 invdir = 1.0 / r.direction;

    vec3 f = (AABB.minimum - r.origin) * invdir;
    vec3 n = (AABB.maximum - r.origin) * invdir;

    vec3 tmax = max(f, n);
    vec3 tmin = min(f, n);

    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    float t0 = max(tmin.x, max(tmin.y, tmin.z));

    return (t1 >= t0) ? ((t0 > 0.0) ? (t0) : (t1)) : (-1);
}
//BVH搜索
bool BvhHit(Ray ray,int now,float t_min,float t_max,inout HitResult rec) {
	int res[1024], nowResult, rightResult, leftResult;
	Node nowNode, RNode, LNode;
	int point = 0;
	res[0] = 1;
	rec.distance1=10000000000000000.0;
	while(point >= 0){
		nowResult = res[point];
		point--;
		nowNode=getBVH(nowResult);
		
		if(nowNode.TriFlag==1){//子节点三角形
			HitResult r;
			if(TriangleHit(nowNode.Tri,ray,r)){
				if(r.distance1<rec.distance1){
					rec=r;
				}
			}
			continue;
		}
		float d1=0,d2=0;
		//Right-Tree
		rightResult=nowResult*2+1;
		RNode=getBVH(rightResult);
		if(RNode.TriFlag==2) d1=hitAABB(ray,RNode.AABB);
		
		//Left-Tree
		leftResult=nowResult*2;
		LNode=getBVH(leftResult);
		if(LNode.TriFlag==2) d2=hitAABB(ray,LNode.AABB);

		if(RNode.TriFlag==1){
			point++;
			res[point]=rightResult;
		}
		if(LNode.TriFlag==1){
			point++;
			res[point]=leftResult;
		}

		if(RNode.TriFlag!=0&&LNode.TriFlag!=0&&d1>0&&d2>0){
			if(d1<d2){
				point++;
				res[point]=rightResult;
				point++;
				res[point]=leftResult;
			}else {
				point++;
				res[point]=leftResult;
				point++;
				res[point]=rightResult;
			}
		}else if(RNode.TriFlag!=0&&d1>0){
			point++;
			res[point]=rightResult;			
		}else if(LNode.TriFlag!=0&&d2>0){
			point++;
			res[point]=leftResult;			
		}
	}
	return rec.isHit;
}
bool searchHit(Ray ray,out HitResult result) {	
	bool flag=false; 
	return BvhHit(ray,1,0,t_max,result);
}
//cosPDF------------------------------------------------------------
//初始化cosPDF的uvw
void init_pdf(vec3 w, inout ONB onb) {
	build_from_w(w, onb);
}
float value_pdf(int pdf[2],vec3 direction, ONB uvw[2]) {
	if(pdf[0]==1&&pdf[1]==0){
		float cosine = dot(normalize(direction), uvw[0].axis[2]);
		return cosine<=0 ? 0 : cosine / PI;
	}
	if(pdf[0]==2&&pdf[1]==0){
		return 1 / (4 * PI);
	}
	float cou=0.0;
	for(int i=0;i<2;i++){
		if(pdf[i]==1){
			float cosine = dot(normalize(direction), uvw[i].axis[2]);
			cou += (cosine<=0 ? 0 : cosine / PI) * 0.5;		
		}
		if(pdf[i]==2){
			cou += (1 / (4 * PI))*0.5;
		}
	}
	return cou;
}
vec3 generate_pdf(ONB uvw) {
	vec3 local = random_cosine_direction();
	return local.x * uvw.axis[0] + local.y * uvw.axis[1] + local.z * uvw.axis[2];
}
float light_pdf(vec3 origin, vec3 v) {
	HitResult rec;
	Ray ray;
	ray.origin = origin;
	ray.direction = v;
	if(!searchHit(ray,rec)) return 0.0;

	float area = (Light.x1 - Light.x0) * (Light.z1 - Light.z0);//发光面积
	float distance_squared = v.x*v.x+v.y*v.y+v.z*v.z;
	float cosine = abs(dot(v,rec.normal) / length(v));

	return distance_squared / (cosine * area);
}
//BPBR-----------------------------------------------------------
//Christophe Schlick玻璃反射计算
float reflectance(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}
//绝缘体pbr Matrial=1
bool DielectricScatter(Ray ray, HitResult rec, inout vec3 Color, out Ray scattered, out scatterRecord srec) {
	float ir=1.5;
	srec.isSpecular = true;
	srec.attention = vec3(1.0, 1.0, 1.0);
	float refraction_radio = rec.frontFace ? (1.0 / ir) : ir;

	vec3 unit_direction = normalize(ray.direction);
	float cos_theta = min(dot(-unit_direction,rec.normal), 1.0);
	float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	bool cannot_refract = refraction_radio * sin_theta > 1.0;
	vec3 direction;
	if(cannot_refract || reflectance(cos_theta, refraction_radio) > rand()){
		direction = reflect(unit_direction, rec.normal);
	}else{
		direction = refract(unit_direction, rec.normal, refraction_radio);
	}
	srec.specularRay.origin= rec.hitPoint;
	srec.specularRay.direction = direction;
	return true;
}
//完全漫反射pbr Matrial=2
bool LambertianScatter(Ray ray, HitResult rec, inout vec3 Color, out Ray scattered, out scatterRecord srec) {
	srec.isSpecular = false;
	srec.attention = Color;
	init_pdf(rec.normal,srec.onb);//cospdf
	srec.attention = rec.Color;
	return true;
}
float LambertianScatter_Pdf(Ray ray, HitResult rec, Ray scattered ) {
	float cosine = dot(rec.normal, normalize(scattered.direction));
	//if(cosine>0)FragColor = vec4(0,0.7,0,1);
	return cosine < 0 ? 0 : cosine / PI;
}
//金属pbr Matrial=3
bool MetalScatter(Ray ray, HitResult rec, inout vec3 Color, out Ray scattered, out scatterRecord srec) {
	float fuzz=0;
	vec3 reflected = reflect(normalize(ray.direction), rec.normal);
	srec.specularRay.origin = rec.hitPoint;
	srec.specularRay.direction = reflected + fuzz * SampleHemisphere();
	srec.isSpecular = true;
	srec.attention = rec.Color;
	return true;
}
bool Scatter(Ray ray, HitResult rec, inout vec3 Color, out Ray scattered, out scatterRecord srec) {
	if(rec.Matrial==1) return DielectricScatter(ray, rec, Color, scattered, srec);
	if(rec.Matrial==2) return LambertianScatter(ray, rec, Color, scattered, srec);
	if(rec.Matrial==3) return MetalScatter(ray, rec, Color, scattered, srec);
	if(rec.Matrial==4) {
		Color = rec.Color;
		return false;
	}
}
bool Scatter_Pdf(Ray ray, HitResult rec, inout vec3 Color, out Ray scattered){
	return false;
}
vec3 traceRay(Ray inputRay, vec3 Color) {
	tracer nowTracer;
	HitResult rec;
	Ray outray;
	vec3 color, outColor = Color, history = Color;
	outColor=vec3(0,0,0);
	nowTracer.ray = inputRay;
	float prob = 0.8, p;
	for(int i=0;i<15;i++){//深度
		p=rand();
		if(p>prob){
			break;
		}
		if(searchHit(nowTracer.ray, rec)){
			scatterRecord srec;
			if(rec.Matrial == 4){
				outColor = history * rec.Color/prob;
				break;
			}
			Scatter(nowTracer.ray, rec, color, outray, srec);
			if(srec.isSpecular){
				nowTracer.ray = srec.specularRay;
				history = (history * srec.attention/prob);
				continue;
			}
			Ray scattered;
			scattered.origin = rec.hitPoint;
			if(rand()<0.5){//间接光照采样
				scattered.direction = generate_pdf(srec.onb);
			}else{//直接光照采样
				vec3 random_point = vec3(random_double(Light.x0, Light.x1), Light.y, random_double(Light.z0, Light.z1));
				scattered.direction = random_point - rec.hitPoint;//在光源选取射线目标点，计算方向
			}
			float pdf_val;
			float cosine = dot(normalize(scattered.direction), srec.onb.axis[2]);
			pdf_val = (cosine<=0 ? 0 : cosine / PI) * 0.5;	
			pdf_val += light_pdf(rec.hitPoint, scattered.direction) * 0.5;
			history = history * srec.attention * LambertianScatter_Pdf(nowTracer.ray, rec, scattered)/pdf_val/prob;
			nowTracer.ray = scattered;
		}else return outColor;
	}
	return outColor;
}
void init(){

}
void main(){
	init();
	vec3 LookFrom, LookAt, vup, u, v , w;
	vup = vec3(0,1,0);
	LookFrom = vec3(0,0,0.5);
	LookAt=vec3(0,0,0);
	w = normalize(LookFrom - LookAt);
	u = normalize(cross(vup,w));
	v = cross(w,u);

	
	vec2 AA = vec2((rand()-0.5)/float(width), (rand()-0.5)/float(height));
	Ray ray;
	ray.origin = LookFrom;
	ray.direction = normalize( vec3(pix.xy+AA,0) - LookFrom);
	vec3 bb = texture(normal,texcoords).rgb;
	vec3 pp = texture(position, texcoords).rgb;
	vec3 color = texture(gColor, texcoords).rgb, nColor;
	vec3 eyeTopos = eye - pp;
	if(bb != vec3(0,0,0)){
		//ray.direction = toNormalHemisphere(SampleHemisphere(),bb);
		//ray.origin = pp;
		ray.direction = -eyeTopos;
		ray.origin = eye;
		HitResult rec;
		if(searchHit(ray,rec)){
//			if(rec.Matrial==2){
//				ray.direction = toNormalHemisphere(SampleHemisphere(),rec.normal);
//				ray.origin = rec.hitPoint;
//			}
			nColor=traceRay(ray, rec.Color);
			if(isnan(nColor.x)||isinf(nColor.x)) nColor.x=0.0;
			if(isnan(nColor.y)||isinf(nColor.y)) nColor.y=0.0;
			if(isnan(nColor.z)||isinf(nColor.z)) nColor.z=0.0;
		}
		else nColor = vec3(0, 0, 0);
	}
	vec4 lastColor = vec4(texture(LastColor, texcoords).rgb, 1.0);
	//FragColor = vec4(0.5,0,0,1);
	//FragColor = mix(lastColor, vec4(nColor, 1.0), 1.0/float(frameCounter));
	FragColor = lastColor + vec4(nColor, 1.0)*0.01;
	if(frameCounter == 2) FragColor = vec4(nColor,1.0)*0.01;
	//FragColor = vec4(2.0/0,0,0,1.0);

}

