#pragma once
#include "geometry.h"
#include "xgeombase.h"
#include "xcloud.h"

#include <vector>
using namespace std;


class AFX_EXT_CLASS XFacet
	{
	public:
		PNT	 m_Vertex[3];
		PNT	 m_Norm;
	public:
		bool IsPnt_in_Triangle(const PNT& pnt) const;
		bool LineSeg_Plane(
			const PNT& pnt1,
			const PNT& pnt2, 
			const PNT& planepnt,
			const PNT& planenorm,
			PNT& interpnt);
		XFacet(void);
		~XFacet(void);

	public:
		XFacet&		 operator = (const XFacet& facet);
		bool Int_Plane(
		       const PNT&  norm, 
		       const PNT&  PntOn , 
		       int& num_int, 
		       float Int_pnt[9]);

		bool			Pnt2Facet_Dist(
							const PNT&	comparepnt, 
							PNT&		nearestpnt, 
							double &			dist, 
							const double&	tolerance)const;
		double		 Pnt2LineSeg(
							const PNT&  Comp, 
							const PNT&  startpnt, 
							const PNT& endpnt, 
							PNT& nearestpnt)const;
		void			GetCenterPnt(PNT& pnt) const;
		/* ------------------two facet  conect?---------------------------*/
		bool			IsNeighbor(						// if neighbor , return true; else return false;
					const XFacet&		facet1,		// the other facet
					int&					index0,		//as for this facet, conect position: for type 0, is vertex index, for type 1, is edge index
					int&					index1,		// as for facet1, conect position: for type 0, is vertex index, for type 1, is edge index
					int &				type                // conection type: 0 for vertex conection; 1 for edge conection; 2 for intersection(topology error)
					)const;


	};

class AFX_EXT_CLASS XMesh: public XGeomBase  
{

protected:
	std::vector<XFacet>	m_Facet;
	long					m_Num;	// facet number in mesh
public:
	XMesh();
	XMesh(const XMesh& mesh);
	virtual ~XMesh();
	virtual void SetUnit(const int& unit);
	virtual void InitObject() ;
protected:
	virtual void	UpdateBndBox();

public:
	void ChangeUnits(bool bmm2inch = true);
	
	bool Load_STL_File( CFile*& loadfile);
	void Draw(void);
	int GetFacetNum(void)const;
	void AddFacet(const XFacet& facet);
	
	void DeleteFacet(int index);

	void ClearMesh(void);
	void GetFacet(const int& index, XFacet& facet)const;
	//double Cloud2Mesh_Dist( XCloud& cloud, XCloud& nearestcld, const double& tolerance, int& num_withintol) const ; 
	//double XMesh::Mesh_ErrorEvaluation(
	//			   XCloud& cloud, 
	//			   XErrorCloud& nearestcld, 
	//			   const double& tolerance
	//			) const ;

	double Pnt2NearestMeshPnt(const PNT& refpnt, const double& tolerance , PNT& nearestpnt) const;
	/*find the boundary edge list  */
	//bool SeekEdges(
	//	std::vector<XCloud>& edgelist , // should be closed polylines edgelist, for one mesh , there are maybe several edgelist
	//	std::vector<bool>& type_edge   // type 0: outer boundary edge list; type 1: inner boundary edgelist(hole)
	//	);
	//bool SeekEdges(
	//	PNT& view_dir,
	//	std::vector<XCloud>& edgelist , // should be closed polylines edgelist, for one mesh , there are maybe several edgelist
	//	std::vector<bool>& type_edge   // type 0: outer boundary edge list; type 1: inner boundary edgelist(hole)
	//	);

	bool GetVisibalMesh(const PNT& view_dir, const double & view_angle, XMesh& ex_mesh);

	/* line constrained registration */
	//int Cloud2Mesh_Reg(
	//		XCloud &			cloud,			// input/output: which will be moved toward mesh model
	//		const PNT&		pnt_axis,		// input: one point on axis
	//		const PNT&		dir,				// input: axis direction
	//		double&				rotation_angle,	// output: rotation angle around axis
	//		double&				trans,			// output: translation along axis direction
	//		double &				rms_error		// output: Root mean square error
	//		);

	void Rotate(
		const PNT	& pnt_axis,	 // point on axis
		const PNT	& dir,		// unit direction vector
		const double		& angle		// rotation angle in rad
		);
	/*  */
	bool Optimize_ViewDir(
		PNT&		view_dir,
		double &			tilt_angle,
		double &			Q_angle,
		bool				bConstr
		);
/*
	friend int __cdecl JacobianforMeshReg(
			  int m,		// number of points 
			  int n,		// number of parameters =2
			  double *const  par,	// the elements are x,y,z, r
			  double*& rv,   // residual value,e.g. distance diviation
			  double **jac,
			  const XCloud& g_pnt,
			  const XMesh&  g_mesh
			  );
	friend int __cdecl EvaluateforMeshReg(
			int		m,
			int		n,
			double *const par, 
			double* rv, 
			  const XCloud& g_pnt,
			  const XMesh&  g_mesh
		       );*/

	void SetDisplay(bool bShow, int dis_type = -1);
	bool Load_STL_File(CArchive & ar);
	bool Save_STL_File(CArchive & ar) const;
	void Translate(const PNT& trans);
	void Rotate(const Ti_Rotation& R);

	XMesh operator - (const XMesh& mesh) const;
	private:
	//void DivideMeshEdge(const XCloud& edge,
	//	const vector<int> index,
	//	std::vector<XCloud>& edgelist,
	//	std::vector<bool>& type_edge);
	//void DivideMeshEdge(const XCloud& edge,
	//	PNT& view_dir,
	//	std::vector<XCloud>& edgelist,
	//	std::vector<bool>& type_edge);


public:
	XMesh HidenFacet(const PNT& dir) const;
//	void TransForm(const Ti_Matrix<double>& RegM);
	void MeshMerge(const XMesh& mesh);
	// just for one model view planning 
	XMesh VME_Planning(const PNT& axis_dir, const PNT& axis_pnt, const PNT& laser_dir, double max_incidence_angle, double& Q_angle);  
	void GetBestViewDir(PNT& dir);
protected:
	int GetPotentialVM(const PNT& view_dir, double max_incidenceangle, XMesh& ex_mesh);
public:
	XCloud GetFacetCenterCld(void);
	friend bool	__cdecl SortCompareMX(const XFacet&  elem1, const XFacet&  elem2);
	friend bool	__cdecl SortCompareMY(const XFacet&  elem1, const XFacet&  elem2);
	friend bool	__cdecl SortCompareMZ(const XFacet&  elem1, const XFacet&  elem2);
	friend int	__cdecl MarquardtforMeshReg(
			   int  m,        //<I>  the number of equations ,in other words, the measured points
			   int    n,        //<I>  the number of unknown variables; n should satisfy n<=m
			   const XCloud&  g_pnt,//<I>  the input points 
			   const XMesh&  g_mesh, // <I> the mesh for reference object
			   double* const par,  //<IO> input: the approximation to a least squares solution of 
								  // the system,output: the calculated least squares solution
			   double*& g,    //<O>  the residual vector at the calculated solutions
			   double**v,    //<O>  the inverse of the matrix J'J where J denote the transpose of 
							// the matrix of partial derivatives g'(i)/par'(j)
			   int (*funct)(int  m,  int n, double* const par, double* g, const XCloud&  g_pnt, const XMesh&  g_mesh ), 
			                 // m,n have the same meaning as above
							 // array par[] contains the current values of the unknowns and should not be altered 
							 // exit: upon completion of a call of funct, the array g[1:m] should contain the 
							 // residual vector obtained with the current values of the unknowns; 
							 // e.g. in curve fitting problems, 
							 // g[i]= theoretical value f(x,par) - observed value y[i]
			                 // after a successful call of funct, the Calculator should deliver the value nonzero; 
							 // however, if funct delivers the value zero then it is assumed that the current 
							 // estimates of the unknowns lie outside a feasible region and the process is 
							 // terminated ; hence , proper programming of funct makes it possible to 
							 // avoid calculation of a residual vector with values					
							 // of the unkonwn variables which make no sense or which even may 
							 // cause overflow in the computaion;
			   int (*jacobian)(int  m, int n , double* const par, double*& g, double **jac, const XCloud&  g_pnt,  const XMesh&  g_mesh),
			                 // m,n par see funct
							 // g contains the residual vector obtained  with the current values of the unknonwns and
							 // should not be altered ;
							 // exit: upon completion of a call of jacobian , the array jac should contain the partial
							 // derivatives g'(i)/par'(j), obtained with the current values of the unknown variables 
							 // given in par[1:n]. it is a prerequisite for the proper operation of the procedure 
							 // marquardt that the precision of the elements of the matrix jac is at least the precision
							 // defined by in[3] and in[4];
			   double in[],   // <I>
			                 // in[0]: the machine precision
							 // in[1],in[2]: these are not used by marquardt
							 // in[3]: the relative tolerance for the difference between 
							 // the Euclidean norm of the ultimate and penultimate residual vector 
							 // in[4]: the absolute tolerence for the difference between 
							 // the Euclidean norm of the ultimate and penultimate residual vector
							 // the process is terminated if the improvement of the sum of squares is 
							 // less than in[3]*(sum of squares)+in[4]*in[4]; 
							 // these tolerence should be chosen greater than the corresponding 
							 // errors of the caculated residual vector
							 // in[5]: the maximum number of calls of funct allowed
							 // in[6]: a starting value used for the relation between 
							 // the gradient and the Gauss-Newton direction (value ¦Îabove);
							 // if the problem is well conditioned then in[6] should be greater,
							 // but the value of in[6] should satisfy:in[0]<in[6]<=1/in[0]

			   double out[]   //<O>
			                 // out[1]: this value gives information about the termination of the process
								// out[1] = 0: normal termination
								// out[1] = 1: the process has been broken off because the number of calls of funct exceeded the number given in in[5]
								// out[1] = 2: the process hase been broken off because a call of funct delivered the value zero
								// out[1] = 3: funct bacame zero when called with the initial estimates of par[1:n];
							           // the interation process was not broken off because the precision asked for cannot be attained ; this precision is possibly chosen too high,relative to the precision in wich the residual vector is calculated
							    // out[1] = 4: the process has been broken off because the precision asked for cann't be attained ; this precision
								//        is possibly chosen too high,relative to the precision in which the residual vector is calculated 
							 // out[2]: the Euclidean norm of the residual vector calculated with values of the unknowns delived
							 // out[3]: the Euclidean norm of the residual vector calculated with the initial values of the unknowns variables
							 // out[4]: the number of calls of funct necessary to attain the calculated result
							 // out[5]: the total number of iterations performed:note that in each interation one evaluation of the Jacobian matrix had to be made
							 // out[6]: the improvement vector in the last interation step
							 // out[7]: the condition number of J'J ,i.e. the ratio of its largest to smallest eigenvalues
			   );

	void Sort_ByAxis(int axis_index);
	void SortByDir(PNT& dir);
	bool Slicing( PNT& section_pnt,  PNT& section_dir, double tolerance , XCloud& section_cld);
	XCloud GetGuassMap(void);
};

