


#ifdef __APPLE__
#include <GLUT/glut.h>
#else

#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "reader.h"
#include "Matrices.h"
#include "glut.h"
#include "math.h"
int flag = 0;
Vector4 cubept1,cubept2,cubept3,cubept4,cubept5,cubept6,cubept7,cubept8,cubept9,cubept10,cylpt1,cylpt2,cylpt3,cylpt4;
float angle;
int toggle = 0;
int rot;
float gamma;

	
int xx = 0;
struct PointNormal 
{
   float x1, y1, z1, w1;
};
float r = 5;
PointNormal n,ppp;
	std::vector<Point> pointscube;//vectors for all the points
	std::vector<Point> pointsv;
	std::vector<Point> pointsvf;
	std::vector<Point> pointsvcyl;
	std::vector<Point> pointspp;
	std::vector<Point> pointssp;
	std::vector<PointNormal> pointsn;
	std::vector<PointNormal> pointsf;
	std::vector<PointNormal> pointscyl;
	std::vector<PointNormal> pointsvpp;
	std::vector<PointNormal> pointsvssp;
	std::vector<PointNormal> pointsvcube;
	Point p;
Point f;
Point c;
Point pp;

int NodeCounter = 0;
int faceCounter = 0;

void openFile(const char * fileName) {



  xmlNode *currPtr;
  xmlNode *currNode;

  doc = xmlReadFile(fileName, NULL, 0);

  head = xmlDocGetRootElement(doc);



  if(doc == NULL){
    printf("error: could not parse file %s\n", fileName);
  }

//  printf("Right at %s \n",head->name);

  if (head->type == XML_ELEMENT_NODE && strcmp((const char*) head->name, "Project2") == 0) {
    for (currPtr = head->children; currPtr != NULL; currPtr = currPtr->next) {

      if (currPtr->type == XML_ELEMENT_NODE && strcmp((const char*) currPtr->name, "Wireframe") == 0) {
	for (currNode = currPtr->children; currNode; currNode = currNode->next) {
	  if(currNode->type == XML_ELEMENT_NODE && strcmp((const char*) currNode->name,"NumPoints") == 0) {
	    nodeCount = atoi((char*) currNode->children->content);
	    //printf("nodeCount %d \n",nodeCount);
	  }
	}
      } else if (currPtr->type == XML_ELEMENT_NODE && strcmp((const char*) currPtr->name, "LineDraw") == 0) {
	for (currNode = currPtr->children; currNode; currNode = currNode->next) {
	  if(currNode->type == XML_ELEMENT_NODE && strcmp((const char*) currNode->name,"NumPoints") == 0) {
	    faceCount = atoi((char*) currNode->children->content);
	    //printf("faceCount %d \n",faceCount);
	  }
	}
      }

    }
  }

  if (nodeCount > 0) {
    nodeList = (Node*)malloc(sizeof(struct Node) * nodeCount);
    //printf("Node size initialized to %d value \n",nodeCount);
  }else {
    printf("Warning: node size has not been initialized \n");
  }

  if (faceCount > 0 ) {
    faceList = (Face*)malloc(sizeof(struct Face) * faceCount);
   // printf("face points initialized to %d \n",faceCount);
  }else {
    printf("Warning: face size has not been initialized");
  }


}

void parseDatabase() {


   xmlNode *currPtr;
   xmlNode *currNode;


    if (doc == NULL) {
      printf("error: xml pointer is NULL ");
      return;
    }

    if (head->type == XML_ELEMENT_NODE && strcmp((const char*) head->name, "Project2") == 0) {
        for (currPtr = head->children; currPtr; currPtr = currPtr->next) {


	  // Reading cononical volume paramaters, umin,umax,vmin,vmax,FrontPlane,BackPlane
	  parseCononicalVolume(currPtr);

          // Reads VPN,VUP,VRP,PRP
	  parseVRC(currPtr);



          // Wireframe tag
	  if (currPtr->type == XML_ELEMENT_NODE && strcmp((const char*) currPtr->name, "Wireframe") == 0) {
	    for (currNode = currPtr->children; currNode; currNode = currNode->next) {

               parseNode(currNode,8);                // Starts with the tag "Coordinates3D" to read the Point data
               parseColor(currNode,8);               // Start with "OutlineColor" or "FillColor" to read the Color data
               parseLineConnectivity(currNode,8);    // Starts with "LineConnectivity to read the Connectivity data
	    }

	  }

          //Sphere
	  else if (currPtr->type == XML_ELEMENT_NODE && strcmp((const char*) currPtr->name, "Sphere") == 0) {
	    for (currNode = currPtr->children; currNode; currNode = currNode->next) {
                parseColor(currNode,6);
                parseRadius(currNode,6);  // Reads the "Radius" value
                parseCenter(currNode,6);  // Starts with the "Center" value to parse the point

	    }

	  }

          //Cylinder
	  else if (currPtr->type == XML_ELEMENT_NODE && strcmp((const char*) currPtr->name, "Cylinder") == 0) {
	    for (currNode = currPtr->children; currNode; currNode = currNode->next) {

                parseColor(currNode,10);
                parseRadius(currNode,10);
	        parseCenter(currNode,10);
                parseHeight(currNode,10);

	    }

	  }

	  // LineDraw
	  else if (currPtr->type == XML_ELEMENT_NODE && strcmp((const char*) currPtr->name, "LineDraw") == 0) {
	    for (currNode = currPtr->children; currNode; currNode = currNode->next) {
	      parseColor(currNode,9);
	      parseNode(currNode,9);
	    }
	  }

	}
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
   

}

void printDatabase() {
  
  int i = 0;
  Connectivity *ptr = NULL;
  if (nodeCount > 0) {
    //printf("------ Node Points -----------\n");
    for (i = 0; i < nodeCount; i++) {

      printf("Point %d :  { %f, %f,%f } \n ",i+1, nodeList[i].point.x,nodeList[i].point.y,nodeList[i].point.z);
    }
  }

  if (faceCount > 0) {
    //printf("------ Face Points -----------\n");
    for (i = 0; i < faceCount; i++) {

      printf("Point %d :  { %f, %f,%f } \n ",i+1, faceList[i].point.x,faceList[i].point.y,faceList[i].point.z);
    }
  }

  i = 1;
  for (ptr = headEdgePtr; ptr != NULL; ptr = ptr->next,i++) {
      printf("Line %d: %d -> %d \n",i,ptr->id1,ptr->id2);
  }

  // volume
  printf("----------- Volume PARAMATERS --------------\n\n");
  printf("umin: %f  umax: %f vmin: %f vmax: %f \n",volume.uMin,volume.uMax,volume.vMin,volume.vMax);
  printf("front plane: %f back plane %f \n",volume.zMin,volume.zMax);

  printf("---------- VRC PARAMATERS ------------\n\n");
  printf("vrp: %f,%f,%f  \n",vrc.vrp.x,vrc.vrp.y,vrc.vrp.z);
  printf("vpn: %f,%f,%f  \n",vrc.vpn.x,vrc.vpn.y,vrc.vpn.z);
  printf("vup: %f,%f,%f  \n",vrc.vup.x,vrc.vup.y,vrc.vup.z);
  printf("prp: %f,%f,%f  \n",vrc.prp.x,vrc.prp.y,vrc.prp.z);


  //Sphere
  printf("--------------SPHERE--------------------\n\n");
  printf("Center: %f,%f,%f \n",sphere.center.x,sphere.center.y,sphere.center.z);
  printf("Color:  %f,%f,%f \n",sphere.color.r,sphere.color.g,sphere.color.b);
  printf("Radius:  %f \n",sphere.radius);

  //Cylinder
  printf("--------------CYLINDER--------------------\n\n");
  printf("Center: %f,%f,%f \n",cylinder.center.x,cylinder.center.y,cylinder.center.z);
  printf("Color:  %f,%f,%f \n",cylinder.color.r,cylinder.color.g,cylinder.color.b);
  printf("Radius:  %f \n",cylinder.radius);
  printf("Height:  %f \n",cylinder.height);

}

void parseCenter(xmlNode *node, int type) {
  xmlNode *currNode;

  if (node->type == XML_ELEMENT_NODE && strcmp((const char*) node->name,"Center") == 0) {
    for(currNode = node->children; currNode != NULL; currNode = currNode->next) {
      parsePoints(currNode,type);
    }
  }
 
}

void parseCononicalVolume(xmlNode *ptr) {
    
  	  if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"umin") == 0) {
	    volume.uMin = atof((char*) ptr->children->content);
	  } else if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"umax") == 0 ) {
	    volume.uMax = atof((char*) ptr->children->content);
	  } else if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"vmin") == 0) {
	    volume.vMin = atof((char*) ptr->children->content);
	  } else if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"vmax") == 0) {
	    volume.vMax = atof((char*) ptr->children->content);
	  }  else if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"FrontPlane") == 0) {
	    volume.zMin = atof((char*) ptr->children->content);
	  }  else if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"BackPlane") == 0) {
	    volume.zMax = atof((char*) ptr->children->content);
	  }
     
}

void parseVRC(xmlNode *ptr) {
    

    xmlNode *currNode = NULL;
  	  if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"PRP") == 0) {
	      for (currNode = ptr->children; currNode; currNode = currNode->next)
		parsePoints(currNode,2);
              //printf("PRP: %f,%f,%f \n",vrc.prp.x,vrc.prp.y,vrc.prp.z);
	  } else if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"VPN") == 0 ) {
	    for (currNode = ptr->children; currNode; currNode = currNode->next)
	      parsePoints(currNode,0);
           // printf("VPN: %f,%f,%f \n",vrc.vpn.x,vrc.vpn.y,vrc.vpn.z);
	  } else if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"VUP") == 0) {
	    for (currNode = ptr->children; currNode; currNode = currNode->next)
	      parsePoints(currNode,1);
           // printf("VUP: %f,%f,%f \n",vrc.vup.x,vrc.vup.y,vrc.vup.z);
	  } else if (ptr->type == XML_ELEMENT_NODE && strcmp((const char*) ptr->name,"VRP") == 0 ) {
	    for (currNode = ptr->children; currNode; currNode = currNode->next)
	      parsePoints(currNode,3);
           // printf("VRP: %f,%f,%f \n",vrc.vrp.x,vrc.vrp.y,vrc.vrp.z);
          }
    
}


void parseNode(xmlNode *node, int type) {
    
    xmlNode *currPtr;
    NodeCounter = 0;
    if (strcmp((const char *) node->name,"Coordinates3D") != 0 ) return;

        if (node->type == XML_ELEMENT_NODE) {
            for (currPtr = node->children; currPtr;currPtr = currPtr->next) {
               if (type == 8) parsePoints(currPtr,8);
               if (type == 9) parsePoints(currPtr,9);
            }

        }


}

void parseLineConnectivity(xmlNode *node,int type) {
   
  int point1, point2;
  xmlNode *currPtr;
  if (strcmp((const char*) node->name,"LineConnectivity") != 0) return;

  for (currPtr = node->children; currPtr; currPtr = currPtr->next) {
      if (currPtr->type == XML_ELEMENT_NODE && strcmp((const char*) currPtr->name,"id") == 0){
	point1 = atoi((char*) currPtr->children->content);

      // Go to the next id
      currPtr = currPtr->next;
      if (currPtr == NULL || strcmp((const char*) currPtr->name,"id") != 0) {
	printf("Expected listing of id2 in lineConnectivity");
      }

      if (currPtr->type == XML_ELEMENT_NODE && strcmp((const char*) currPtr->name,"id") == 0)
	point2 = atoi((char*) currPtr->children->content);

      if (type == 8 ) {
            if (headEdgePtr == NULL) {
            headEdgePtr = (Connectivity*)malloc(sizeof(struct Connectivity));
            headEdgePtr->next = NULL;
            headEdgePtr->id1 = point1;
            headEdgePtr->id2 = point2;
            tailEdgePtr = headEdgePtr;
        }else {
            tailEdgePtr->next = (Connectivity*)malloc(sizeof(struct Connectivity));
            tailEdgePtr = tailEdgePtr->next;
            tailEdgePtr->id1 = point1;
            tailEdgePtr->id2 = point2;
            tailEdgePtr->next = NULL;
        }
     }
      }
  }
     
}

void parseRadius(xmlNode *node, int type) {
    
  float radius;
  int isEvaluated = 0;
  if (node->type == XML_ELEMENT_NODE && (isEvaluated==0)&& strcmp((const char*) node->name,"Radius") == 0) {
    radius = atof((char*) node->children->content);
    isEvaluated=1;
  }

  if (type == 10 && isEvaluated==1) {
    cylinder.radius = radius;
  } else if (type == 6&& isEvaluated==1) {
    sphere.radius = radius;
  }
    
}

void parseHeight(xmlNode *node, int type) {

  float height;
  int isEvaluated = 0;
  if (node->type == XML_ELEMENT_NODE && (isEvaluated==0)&& strcmp((const char*) node->name,"Height") == 0) {
    height = atof((char*) node->children->content);
    isEvaluated=1;
  }

  if (type == 10 && isEvaluated==1) {
    cylinder.height = height;
  }

}

void parseColor(xmlNode *node, int type) {
    
    float r = 0;
    float g = 0;
    float b = 0;
  xmlNode *currNode;
  xmlNode *color1Node;

  for (color1Node = node; color1Node; color1Node = color1Node->next){

  if (color1Node->type == XML_ELEMENT_NODE && strcmp((const char*) color1Node->name, "OutlineColor") == 0) {
    xmlNode *colorNode;
    for (colorNode = color1Node->children; colorNode; colorNode = colorNode->next) {
        if (colorNode->type == XML_ELEMENT_NODE && strcmp((const char*) colorNode->name,"Color") == 0) {
         for (currNode = colorNode->children; currNode; currNode = currNode->next) {

	   if (currNode->type == XML_ELEMENT_NODE && strcmp((const char*) currNode->name,"Red") == 0) {
	       r = atof((char*) currNode->children->content);

	   }

	   if (currNode->type == XML_ELEMENT_NODE && strcmp((const char*) currNode->name,"Green") == 0) {
	    g = atof((char*) currNode->children->content);
	   }

	   if (currNode->type == XML_ELEMENT_NODE && strcmp((const char*) currNode->name,"Blue") == 0) {
	       b = atof((char*) currNode->children->content);
	   }

        }

      if (type == 10 ) {
	cylinder.color.r = r;
	cylinder.color.g = g;
	cylinder.color.b = b;
      }else if (type == 6 ) {
	sphere.color.r = r;
	sphere.color.g = g;
	sphere.color.b = b;
      }else if (type == 8 ) // WireFrame
      {
          nodeList[0].color.r  =r;
          nodeList[0].color.g  =g;
          nodeList[0].color.b  =b;
      }
      else if (type =9 )  //LineDraw
      {
          faceList[0].color.r = r;
          faceList[0].color.g = g;
          faceList[0].color.b = b;
      }
    }
  }
  }
  }
    

}


void parsePoints(xmlNode *node, int type) {
    
  float x,y,z = 0;
  xmlNode *currNode;
  xmlNode *tmpNode;


  if (strcmp((const char*) node->name,"Point") != 0) {
      return;
  }


    for (currNode = node->children; currNode; currNode = currNode->next) {
      if (currNode->type == XML_ELEMENT_NODE && strcmp((const char*) currNode->name, "x") == 0) {
	x = atof((char*) currNode->children->content);
     //   printf("x: %f \n",x);
      } else if (currNode->type == XML_ELEMENT_NODE && strcmp((const char*) currNode->name, "y") == 0) {
        y = atof((char*) currNode->children->content);
     //   printf("y: %f \n",y);
      } else if (currNode->type == XML_ELEMENT_NODE && strcmp((const char*) currNode->name, "z") == 0) {
	z = atof((char*) currNode->children->content);
     //   printf("z: %f \n",z);
      }
    }
      if (type == 0 ) {
      //VPN
	vrc.vpn.x = x;
	vrc.vpn.y = y;
	vrc.vpn.z = z;
      }else if (type == 1 ) {
     //VUP
	vrc.vup.x = x;
	vrc.vup.y = y;
	vrc.vup.z = z;
      }else if (type == 2)  {
     //PRP
	vrc.prp.x = x;
	vrc.prp.y = y;
	vrc.prp.z = z;
      }else if (type == 3 ) {
     //VRP
	vrc.vrp.x = x;
	vrc.vrp.y = y;
	vrc.vrp.z = z;
      }else if (type == 8 ) {
      //Wireframe
          if (NodeCounter >= nodeCount) {
              printf("Error reading past the point list for WIREFRAME");
              exit(0);
          }
	nodeList[NodeCounter].point.x = x;
	nodeList[NodeCounter].point.y = y;
	nodeList[NodeCounter].point.z = z;
   //     printf("%f %f %f \n",x,y,z);
	NodeCounter++;
      }else if (type == 9 ) {
     //LineDraw
          if(faceCounter >= faceCount) {
              printf("Error reading past the point list for LineDraw");
              exit(0);
          }
	faceList[faceCounter].point.x = x;
	faceList[faceCounter].point.y = y;
	faceList[faceCounter].point.z = z;
	faceCounter++;
      }else if (type == 6 ) {
        //Sphere
	sphere.center.x = x;
	sphere.center.y = y;
	sphere.center.z = z;
      }else if (type == 10 ) {
       //Cylinder
	cylinder.center.x = x;
	cylinder.center.y = y;
	cylinder.center.z = z;
      }

}

void display(void) {
	/* clear the screen to the clear colour */
	glClear(GL_COLOR_BUFFER_BIT);

          glColor3f(1.0,0.0,0.0);
		  glBegin(GL_LINE_LOOP); // in the first lab assignment, you can only use GL_POINT
		  glVertex2f(pointsn[0].x1, pointsn[0].y1);//node points and face points
		  glVertex2f(pointsn[1].x1, pointsn[1].y1);	
		  glVertex2f(pointsn[2].x1, pointsn[2].y1);
		  glVertex2f(pointsn[3].x1, pointsn[3].y1);
		  glVertex2f(pointsn[0].x1, pointsn[0].y1);
		  glEnd();
    
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[4].x1, pointsn[4].y1);
		  glVertex2f(pointsn[5].x1, pointsn[5].y1);
		  glEnd();
    
		  
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[5].x1, pointsn[5].y1);
		  glVertex2f(pointsn[6].x1, pointsn[6].y1);
		  glEnd();
		 
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[7].x1, pointsn[7].y1);
		  glVertex2f(pointsn[6].x1, pointsn[6].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[7].x1, pointsn[7].y1);
		  glVertex2f(pointsn[4].x1, pointsn[4].y1);
		  glEnd();
		  
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[0].x1, pointsn[0].y1);
		  glVertex2f(pointsn[4].x1, pointsn[4].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[1].x1, pointsn[1].y1);
		  glVertex2f(pointsn[5].x1, pointsn[5].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[7-5].x1, pointsn[7-5].y1);
		  glVertex2f(pointsn[4+2].x1, pointsn[4+2].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[7].x1, pointsn[7].y1);
		  glVertex2f(pointsn[3].x1, pointsn[3].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[8].x1, pointsn[8].y1);
		  glVertex2f(pointsn[9].x1, pointsn[9].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[9].x1, pointsn[9].y1);
		  glVertex2f(pointsn[10].x1, pointsn[10].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[10].x1, pointsn[10].y1);
		  glVertex2f(pointsn[11].x1, pointsn[11].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[10].x1, pointsn[10].y1);
		  glVertex2f(pointsn[13].x1, pointsn[13].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[9].x1, pointsn[9].y1);
		  glVertex2f(pointsn[11].x1, pointsn[11].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsn[9].x1, pointsn[9].y1);
		  glVertex2f(pointsn[13].x1, pointsn[13].y1);
		  glEnd();
		   glBegin(GL_LINES);
		  glVertex2f(pointsn[9].x1, pointsn[9].y1);
		  glVertex2f(pointsn[12].x1, pointsn[12].y1);
		  glEnd();
		   glBegin(GL_LINES);
		  glVertex2f(pointsn[12].x1, pointsn[12].y1);
		  glVertex2f(pointsn[13].x1, pointsn[13].y1);
		  glEnd();
		   glBegin(GL_LINES);
		  glVertex2f(pointsn[12].x1, pointsn[12].y1);
		  glVertex2f(pointsn[11].x1, pointsn[11].y1);
		  glEnd();
		  //face points
		  glBegin(GL_LINE_LOOP); 
		  glVertex2f(pointsf[0].x1 ,pointsf[0].y1);
		  glVertex2f(pointsf[1].x1, pointsf[1].y1);
		  glVertex2f(pointsf[2].x1, pointsf[2].y1);
		  glVertex2f(pointsf[3].x1, pointsf[3].y1);
		  glVertex2f(pointsf[0].x1, pointsf[0].y1);
		  glEnd();
		 
		  for(int v = 0;v<4;v++)//plotting cube points
		  {   
			  glBegin(GL_LINES);
			  glVertex2f(pointsvcube[v].x1/pointsvcube[v].w1,pointsvcube[v].y1/pointsvcube[v].w1);
			  glVertex2f(pointsvcube[v+1].x1/pointsvcube[v+1].w1,pointsvcube[v+1].y1/pointsvcube[v+1].w1);
			  glEnd();
		 
		  }
		  for(int v = 5;v<9;v++)
		  {
			  glBegin(GL_LINES);
			  glVertex2f(pointsvcube[v].x1/pointsvcube[v].w1,pointsvcube[v].y1/pointsvcube[v].w1);
			  glVertex2f(pointsvcube[v+1].x1/pointsvcube[v+1].w1,pointsvcube[v+1].y1/pointsvcube[v+1].w1);
			  glEnd();
		  }
		  for(int kk = 0;kk<100;kk++)//plotting cylinder
		  {
		  glBegin(GL_LINES);
		  if(kk<99)
		  {glVertex2f(pointscyl[kk].x1/pointscyl[kk].w1, pointscyl[kk].y1/pointscyl[kk].w1);
		  glVertex2f(pointscyl[kk + 1].x1/pointscyl[kk+1].w1, pointscyl[kk + 1].y1/pointscyl[kk+1].w1);
		  glEnd();}}
		  for(int kk = 0;kk<100;kk++)
		  { glBegin(GL_LINES);
		  glVertex2f(pointscyl[kk + 99].x1/pointscyl[kk + 99].w1, pointscyl[kk + 99].y1/pointscyl[kk + 99].w1);
		  
		  glVertex2f(pointscyl[kk + 100].x1/pointscyl[kk + 100].w1, pointscyl[kk + 100].y1/pointscyl[kk + 100].w1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointscyl[kk].x1/pointscyl[kk].w1, pointscyl[kk].y1/pointscyl[kk].w1);
		  glVertex2f(pointscyl[kk + 100].x1/pointscyl[kk + 100].w1, pointscyl[kk + 100].y1/pointscyl[kk + 100].w1);
		  glEnd();}
		  
		  for(int v = 0;v<5;v++)
		  {
			  glBegin(GL_LINES);
			  glVertex2f(pointsvcube[v].x1/pointsvcube[v].w1,pointsvcube[v].y1/pointsvcube[v].w1);
			  glVertex2f(pointsvcube[v+5].x1/pointsvcube[v+5].w1,pointsvcube[v+5].y1/pointsvcube[v+5].w1);
			  glEnd();
		  }
		  

		  //play piece
		  glBegin(GL_LINES);
		  glVertex2f(pointsvpp[0].x1, pointsvpp[0].y1);
		  glVertex2f(pointsvpp[1].x1, pointsvpp[1].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsvpp[1].x1, pointsvpp[1].y1);
		  glVertex2f(pointsvpp[2].x1, pointsvpp[2].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsvpp[1].x1, pointsvpp[1].y1);
		  glVertex2f(pointsvpp[4].x1, pointsvpp[4].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsvpp[0].x1, pointsvpp[0].y1);
		  glVertex2f(pointsvpp[2].x1, pointsvpp[2].y1);
		  glEnd();
		  glBegin(GL_LINES);
		  glVertex2f(pointsvpp[0].x1, pointsvpp[0].y1);
		  glVertex2f(pointsvpp[4].x1, pointsvpp[4].y1);
		  glEnd();
          glBegin(GL_LINES);
		  glVertex2f(pointsvpp[0].x1, pointsvpp[0].y1);
		  glVertex2f(pointsvpp[3].x1, pointsvpp[3].y1);
		  glEnd();
          glBegin(GL_LINES);
		  glVertex2f(pointsvpp[3].x1, pointsvpp[3].y1);
		  glVertex2f(pointsvpp[4].x1, pointsvpp[4].y1);
		  glEnd();
          glBegin(GL_LINES);
		  glVertex2f(pointsvpp[3].x1, pointsvpp[3].y1);
		  glVertex2f(pointsvpp[2].x1, pointsvpp[2].y1);
		  glEnd();
		  
		  for(double kk = 1;kk<pointsvssp.size();kk = kk +100)
		  {
		  glBegin(GL_POINTS);
		  glVertex2f(pointsvssp[kk].x1, pointsvssp[kk].y1);
		  glEnd();	
		  
			}
		  
		    
		
		
   */ glutSwapBuffers();
    }
const char * filename = "project2.xml";  // Opens the XML file based on the entire pathname "filename"
void reshape (int w, int h) {
	/* set the viewport */
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	/* Matrix for projection transformation */
	glMatrixMode (GL_PROJECTION); 

	/* replaces the current matrix with the identity matrix */
	glLoadIdentity ();

	/* Define a 2d orthographic projection matrix */
	gluOrtho2D (0.0, (GLdouble) w, 0.0, (GLdouble) h);
}





void transform()
{std::cout<<2556569;
	Matrix4 translate(1,0,0,((-1)*(vrc.vrp.x)),
					  0,1,0,((-1)*(vrc.vrp.y)),
					  0,0,1,((-1)*(vrc.vrp.z)),
					  0,0,0,1);//implementing translation by VRP	
	


//Now calculating rotation matrix

    float ax = vrc.vpn.x;
    float ay = vrc.vpn.y;
    float az = vrc.vpn.z;
    float den = sqrt((ax*ax)+(ay*ay)+(az*az));

	float rotelement11 =  ax/den ;//calculating Rz
	float rotelement12 =  ay/den ;
	float rotelement13 =  az/den ;
	float rotelement14 =  0;

	
	Vector3 Rz(rotelement11,rotelement12,rotelement13);
 
    float bx = vrc.vup.x;
    float by = vrc.vup.y;
    float bz = vrc.vup.z;
    Vector3 vu(bx,by,bz);
    Vector3 temp;
    temp = vu.cross(Rz);
    float den2;
    den2 = sqrt((temp.x*temp.x) + (temp.y*temp.y) + (temp.z*temp.z));
    Vector3 Rx((temp.x/den2),(temp.y/den2),(temp.z/den2));
    Vector3 Ry = Rz.cross(Rx);
 

 
    Matrix4 Rotate(Rx.x,Rx.y,Rx.z,0,
                   Ry.x,Ry.y,Ry.z,0,
                   Rz.x,Rz.y,Rz.z,0,
                   0,0,0,1);
 

	Matrix4 prp(1,0,0,((-1)*(vrc.prp.x)),
					  0,1,0,((-1)*(vrc.prp.y)),
					  0,0,1,((-1)*(vrc.prp.z)),
					  0,0,0,1);//implementing translation by PRP	
	


	//shear matrix
    Vector4 cw(((volume.uMax+volume.uMin)/2),((volume.vMax+volume.vMin)/2),0,1);
    Vector4 dop((cw.x - vrc.prp.x ),(cw.y - vrc.prp.y),(cw.z - vrc.prp.z),0);
    float shx = ((-1)*((dop.x)/(dop.z)));
    float shy = ((-1)*((dop.y)/(dop.z)));
    Matrix4 shear(1,0,shx,0,
                  0,1,shy,0,
                  0,0,1,0,
                  0,0,0,1);
	
	
	
	Vector4 zero(0,0,0,1);
	Vector4 tmp1;
	tmp1 = prp*zero;
	
	Vector4 vrpp;
	vrpp = shear*tmp1;
	std::cout<<vrpp.z;
	std::cout<<vrpp.x;
	std::cout<<vrpp.y;
	float sx = 2*vrpp.z/(((volume.uMax-volume.uMin))*(vrpp.z + volume.zMax));
	float sy = 2*vrpp.z/(((volume.vMax-volume.vMin))*(vrpp.z + volume.zMax));
	float sz = -1/(vrpp.z + volume.zMax);
	Matrix4 scale(sx,0,0,0,
			  0,sy,0,0,
			  0,0,sz,0,
			  0,0,0,1);
	

	float zmin = (((-1)*(vrpp.z + volume.zMin))/(vrpp.z + volume.zMax));
	Matrix4 M(1,0,0,0,
		      0,1,0,0,
		      0,0,(1/(1+zmin)),(-zmin/(1+zmin)),
		      0,0,-1,0);

	Matrix4 viewport;
	
	Matrix4 T1(1,0,0,1,
			   0,1,0,1,
			   0,0,1,1,
			   0,0,0,1);
	Matrix4 scaleview(400,0,0,0,
					  0,400,0,0,
					  0,0,0,0,
					  0,0,0,1
		);
	viewport = scaleview*T1;
	//viewport matrix

	
    int k,i1;
    Matrix4 norm;
    Matrix4 translateV1(1,0,0,-20,//Rotation
					  0,1,0,0,
					  0,0,1,22.5,
					  0,0,0,1);
	Matrix4 rotateV(cos(0.0174*gamma),0,sin(0.0174*gamma),0,
					  0,1,0,0,
					  -sin(0.0174*gamma),0,cos(0.0174*gamma),0,
					  0,0,0,1);	
	Matrix4 translateV2(1,0,0,20,
					  0,1,0,0,
					  0,0,1,-22.5,
					  0,0,0,1);
    if(flag == 1)
	{
    norm = rotateV*translateV1;
	norm = translateV2*norm;
			

	norm = translate*norm;
	norm = Rotate*norm;
	
	norm = prp*norm;
	norm = shear*norm;
	norm = scale*norm;
	norm = M*norm;

	norm = viewport*norm;
	std::cout<<norm;
	}
    else
    {
	norm = Rotate*translate;
	
	
	norm = prp*norm;
	norm = shear*norm;
	norm = scale*norm;
	norm = M*norm;
	norm = viewport*norm;
	std::cout<<norm;//Calculating viewport scaling and normalization matrix
    }
	
	


    for (i1 = 0; i1 < nodeCount; i1++)
    {
        p = nodeList[i1].point;
        pointsv.push_back(p);//creating vector of node points
    }
    for (i1 = 9; i1 < nodeCount; i1++)
    {
        pp.x = nodeList[i1].point.x - 15;
        pp.y = nodeList[i1].point.y - 30 ;
        pp.z = nodeList[i1].point.z;
        pointspp.push_back(pp);
    }
    for (i1 = 0; i1 < faceCount; i1++)
    {
        f = faceList[i1].point;
        pointsvf.push_back(f);//creating vector of face points
    }

    for(int ik = 0; ik < 100; ik++)
    {
        angle = ik*2*3.14/100; 
		Vector4 cylinder1(cylinder.center.x + (cos(angle) * 5), cylinder.center.y,cylinder.center.z + (sin(angle) * 5),1); 
		c.x = cylinder1.x;
        c.y = cylinder1.y;
		c.z = cylinder1.z;
		pointsvcyl.push_back(c);//creating vector of cylinder points
    }

    for(int ik = 0; ik < 100; ik++)
    {
        angle = ik*2*3.14/100; 
		Vector4 cylinder2(cylinder.center.x + (cos(angle) * 5), cylinder.center.y + 10,cylinder.center.z + (sin(angle) * 5),1); 
		c.x = cylinder2.x;
        c.y = cylinder2.y;
		c.z = cylinder2.z;
		pointsvcyl.push_back(c);
    }

    for(float theta = 0;theta<180;theta++)
	 {
         for( float alpha = 0;alpha<180;alpha++)
 
            {
                float x = 2.5*cos(0.0174*theta)*sin(0.0174*alpha);
                float y = 2.5*sin(0.0174*theta)*sin(0.0174*alpha);
                float z = 2.5*cos(0.0174*alpha);

                p.x = sphere.center.x + x;
                p.y = sphere.center.y + y;
                p.z = sphere.center.z + z;
                pointssp.push_back(p);//creating vector of sphere points
 
                p.x = sphere.center.x - x;
                p.y = sphere.center.y + y;
                p.z = sphere.center.z + z;
                pointssp.push_back(p);

                p.x = sphere.center.x + x;
                p.y = sphere.center.y - y;
                p.z = sphere.center.z + z;
                pointssp.push_back(p);

                p.x = sphere.center.x + x;
                p.y = sphere.center.y + y;
                p.z = sphere.center.z - z;
                pointssp.push_back(p);

                p.x = sphere.center.x - x;
                p.y = sphere.center.y - y;
                p.z = sphere.center.z + z;
                pointssp.push_back(p);

                p.x = sphere.center.x - x;
                p.y = sphere.center.y - y;
                p.z = sphere.center.z - z;
                pointssp.push_back(p);
 
                p.x = sphere.center.x + x;
                p.y = sphere.center.y - y;
                p.z = sphere.center.z - z;
                pointssp.push_back(p);
 
                p.x = sphere.center.x - x;
                p.y = sphere.center.y + y;
                p.z = sphere.center.z - z;
                pointssp.push_back(p);
            }
     }

    for (double i1 = 0; i1 < pointssp.size(); i1++)
    {
        
        Vector4 node1(pointssp[i1].x,pointssp[i1].y,pointssp[i1].z,1);
        Vector4 normalizedpt;
        
        normalizedpt = norm*node1;
        
        n.x1 = normalizedpt.x/normalizedpt.w;
        n.y1 = normalizedpt.y/normalizedpt.w;
        n.z1= normalizedpt.z/normalizedpt.w;
        n.w1 = normalizedpt.w/normalizedpt.w;
        pointsvssp.push_back(n);
        
    }
    
    for (i1 = 0; i1 < nodeCount; i1++)
    {
        Vector4 node1(pointsv[i1].x,pointsv[i1].y,pointsv[i1].z,1);
        Vector4 normalizedpt;
        
        normalizedpt = norm*node1;
        
        n.x1 = normalizedpt.x/normalizedpt.w;
        n.y1 = normalizedpt.y/normalizedpt.w;
        n.z1= normalizedpt.z/normalizedpt.w;
        n.w1 = normalizedpt.w/normalizedpt.w;
        pointsn.push_back(n);
        
    }
    
    for (i1 = 0; i1 < 5; i1++)
    {
        Vector4 nodepp(pointspp[i1].x,pointspp[i1].y,pointspp[i1].z,1);
        Vector4 normalizedpp;
        
        normalizedpp = norm*nodepp;
        
        n.x1 = normalizedpp.x/normalizedpp.w;
        n.y1 = normalizedpp.y/normalizedpp.w;
        n.z1= normalizedpp.z/normalizedpp.w;
        n.w1 = normalizedpp.w/normalizedpp.w;
        pointsvpp.push_back(n);
        
    }

    for (i1 = 0; i1 < 200; i1++)
    {
        Vector4 cylnode(pointsvcyl[i1].x,pointsvcyl[i1].y,pointsvcyl[i1].z,1);
        Vector4 normcyl;
        normcyl = norm*cylnode;
        n.x1 = normcyl.x;
        n.y1 = normcyl.y;
        n.z1 = normcyl.z;
        n.w1 = normcyl.w;
        pointscyl.push_back(n);
    }


    Vector4 node2(-15,0,-40,1);//Original wall points
    Vector4 node3(55,0,-40,1);
    Vector4 node4(55,40,-40,1);
    Vector4 node5(-15,40,-40,1);
    Vector4 node6(-15,0,-40,1);
 

    Vector4 node7(-15,0,-5,1);
    Vector4 node8(55,0,-5,1);
    Vector4 node9(55,40,-5,1);
    Vector4 node10(-15,40,-5,1);
    Vector4 node11(-15,0,-5,1);



    p.x = node2.x;
    p.y = node2.y;
    p.z = node2.z;

    pointscube.push_back(p);
    p.x = node3.x;
    p.y = node3.y;
    p.z = node3.z;
    
    pointscube.push_back(p);
    p.x = node4.x;
    p.y = node4.y;
    p.z = node4.z;
    
    pointscube.push_back(p);
    p.x = node5.x;
    p.y = node5.y;
    p.z = node5.z;
    
    pointscube.push_back(p);
    p.x = node6.x;
    p.y = node6.y;
    p.z = node6.z;
    
    pointscube.push_back(p);
    p.x = node7.x;
    p.y = node7.y;
    p.z = node7.z;
    
    pointscube.push_back(p);
    p.x = node8.x;
    p.y = node8.y;
    p.z = node8.z;
    
    pointscube.push_back(p);
    p.x = node9.x;
    p.y = node9.y;
    p.z = node9.z;
    
    pointscube.push_back(p);
    p.x = node10.x;
    p.y = node10.y;
    p.z = node10.z;
    
    pointscube.push_back(p);
    p.x = node11.x;
    p.y = node11.y;
    p.z = node11.z;

    pointscube.push_back(p);
    if(r == 1)
    {
        Vector4 tempv1,temp1;
        tempv1 = translateV1*temp1;
		tempv1 = rotateV*tempv1;
		tempv1 = translateV2*tempv1;
		Matrix4 final;
		for (float i1 = 0; i1 < pointscube.size(); i1++)
		{
            Vector4 tempfinal(pointscube[i1].x,pointscube[i1].y,pointscube[i1].z,1);
            Vector4 tem;
            tem = final*tempfinal;
            pointscube[i1].x = tem.x;
            pointscube[i1].y = tem.y;
            pointscube[i1].z = tem.z;
		}
    }

    for (float i1 = 0; i1 < pointscube.size(); i1++)
    {
        Vector4 cubenode(pointscube[i1].x,pointscube[i1].y,pointscube[i1].z,1);
        Vector4 normcube = norm*cubenode;
        n.x1 = normcube.x;
        n.y1 = normcube.y;
        n.z1 = normcube.z;
        n.w1 = normcube.w;
        pointsvcube.push_back(n);
    }




    for (i1 = 0; i1 < faceCount; i1++)
    {
        
        Vector4 node1(pointsvf[i1].x,pointsvf[i1].y,pointsvf[i1].z,1);
        Vector4 normalizedpt;

        normalizedpt = norm*node1;

        n.x1 = normalizedpt.x/normalizedpt.w;
        n.y1 = normalizedpt.y/normalizedpt.w;
        n.z1= normalizedpt.z/normalizedpt.w;
        n.w1 = normalizedpt.w/normalizedpt.w;
        pointsf.push_back(n);
        pointsvpp.push_back(ppp);

    }


    }

void Keyboard(unsigned char key, int x, int y)
{
	
	switch (key)
	{
		case 't':       //TOGGLE KEY
			
            
            
            toggle++;
            if(toggle==1) //Top view
            {
                vrc.vpn.x=0,vrc.vpn.y=1,vrc.vpn.z=0;
                vrc.vup.x=0,vrc.vup.y=0,vrc.vup.z=-1;
                vrc.vrp.x=20,vrc.vrp.y=60,vrc.vrp.z=-20;
            }
            else if(toggle==2) //Side view
            {
                vrc.vpn.x=1,vrc.vpn.y=0,vrc.vpn.z=0;
                vrc.vup.x=0,vrc.vup.y=1,vrc.vup.z=0;
                vrc.vrp.x=80,vrc.vrp.y=20,vrc.vrp.z=-20;
            }
            else if(toggle==3) //front view
            {
                vrc.vpn.x=0,vrc.vpn.y=0,vrc.vpn.z=1;
                vrc.vup.x=0,vrc.vup.y=1,vrc.vup.z=0;
                vrc.vrp.x=20,vrc.vrp.y=20,vrc.vrp.z=30;
                
            }
            else if(toggle==4) //User-defined view(Left hand side view)
            {
                vrc.vpn.x=-1,vrc.vpn.y=0,vrc.vpn.z=0;
                vrc.vup.x=0,vrc.vup.y=1,vrc.vup.z=0;
                vrc.vrp.x=-20,vrc.vrp.y=20,vrc.vrp.z=-20;
                
            }
            if(toggle==5){//Back to default
                toggle=0;
                vrc.vpn.x=0,vrc.vpn.y=0,vrc.vpn.z=1;
                vrc.vup.x=0,vrc.vup.y=1,vrc.vup.z=0;
                vrc.vrp.x=20,vrc.vrp.y=20,vrc.vrp.z=20;
                
            }
            
            pointsn.clear();
            pointsf.clear();
            pointscyl.clear();
            pointsvpp.clear();
            pointsvssp.clear();
            pointsvcube.clear();
            transform();
            break;
            
            case 'r'://Rotation key
			
			
			rot++;
			
            
            if(rot!=5)
            {flag =1;
                pointsn.clear();
                pointsf.clear();
                pointscyl.clear();
                pointsvpp.clear();
                pointsvssp.clear();
                pointsvcube.clear();
                gamma = gamma +15;
                transform();}
            
			break;
            
            
            case 'o':      // Zoom out
            
			if((vrc.prp.z)!= 5)
			{
                angle = 0;
                
                pointscube.clear();
                pointsv.clear();
                pointsvf.clear();
                pointsvcyl.clear();
                pointspp.clear();
                pointssp.clear();
                
                
                pointscube.clear();
                pointsn.clear();
                pointsf.clear();
                pointscyl.clear();
                pointsvpp.clear();
                pointsvssp.clear();
                pointsvcube.clear();
                xx = xx+5;
                
                
                vrc.prp.z = vrc.prp.z - 1;
                transform();
                std::cout<<vrc.prp.z;}
            
			break;
            
            case 'i':   //Zoom in
            
            if((vrc.prp.z) < 40)
			{
                
                
                angle = 0;
                
                pointscube.clear();
                pointsv;
                pointsvf;
                pointsvcyl;
                pointspp;
                pointssp;
                
                
                pointsn.clear();
                pointsf.clear();
                pointscyl.clear();
                pointsvpp.clear();
                pointsvssp.clear();
                pointsvcube.clear();
                
                
                vrc.prp.z = vrc.prp.z + 1;
                transform();
                std::cout<<vrc.prp.z;}
            
			break;
            
	}
}

int main(int argc, char** argv) {
	
	
    openFile(filename);
    parseDatabase();
 
	transform();

	
	/* deal with any GLUT command Line options */
    glutInit(&argc, argv);

	/* create an output window */
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(800, 800);

	/* set the name of the window and try to create it */
    glutCreateWindow("Perspective Projection");

	/* specify clear values for the color buffers */
	glClearColor (0.0, 0.0, 0.0, 1.0);

    /* Receive keyboard inputs */
    glutKeyboardFunc (Keyboard);

    /* assign the display function */
    glutDisplayFunc(display);

	/* assign the idle function */
    glutIdleFunc(display);

    /* sets the reshape callback for the current window */
	glutReshapeFunc(reshape);

    /* enters the GLUT event processing loop */
    glutMainLoop();




    return(EXIT_SUCCESS);
    
}

