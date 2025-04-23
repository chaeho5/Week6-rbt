#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>


rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)malloc(sizeof(rbtree));//rbtree 구조채 전체의 메모리를 동적 할당
  // TODO: 필요한 경우 구조체 초기화
  if (p != NULL){//트리가 존재 한다면 
    p->nil = (node_t *)malloc(sizeof(node_t));// NIL 노드의 메모리를 동적 할당
    p->root = p->nil; //트리가 비어있음, 루트 초기화
    if (p->nil != NULL){//닐이 존재 한다면 
    p->nil->color = RBTREE_BLACK;
    p->nil->left = p->nil->right = p->nil->parent = p->nil;
    }// NIL 노드는 항상 검정색, rbtree.h에 있는 RBTREE_BLACK 이용
  }
  return p;
}


/*노드를 삭제할 때, 삭제할 노드를 대체할 노드를 연결하기 위해 사용*/
void rbtree_transplant(rbtree* t, node_t* u, node_t* v){//u는 대체될 노드, y는 대체할 노드
  if (u->parent == t->nil){// u가 루트 노드인 경우
    t->root = v;//v를 새로운 루트로 설정
  } else if (u == u->parent->left){//u가 부모의 왼쪽 자녀일 때
    u->parent->left = v;
  } else{ //부모의 오른쪽 자식인 경우
    u->parent->right = v;//부모의 오른쪽 자식을 v로 변경
  }
  v->parent = u -> parent; //v의 부모를 u의 부모로 설정
}
/*노드를 삭제할 때, 삭제할 노드를 대체할 노드를 연결하기 위해 사용*/



/********************x기준 왼쪽 회전**********************/
void left_rotate(rbtree *t, node_t *x) {
  node_t* y = x->right; //y는 x의 오른쪽
  x->right = y->left; //회전 후 y의 왼쪽 서브 트리가 x의 오른쪽으로 감

  if (y->left != t->nil){
    y->left->parent = x;
  }

  y->parent = x->parent;// y가 x의 새로운 부모가 되었으므로 이제 y의 부모를 x의 부모로 변경

  if (x->parent == t->nil){//x가 루트인 경우, y가 루트가 된다
    t->root = y;
  }else if (x == x->parent->left){//x가 부모의 왼쪽 자식인 경우, y가 x자리로 가서 그 부모의 새로운 자식이 됨.
    x->parent->left = y;
  }else {//x가 부모의 오른쪽 자식인 경우, 
    x->parent->right = y;
  }
  y->left = x;//x를 y의 왼쪽 자식으로 만듬
  x->parent = y;//x의 부모를 y로
  // TODO: 왼쪽 회전 구현
}
/********************x기준 왼쪽 회전**********************/


/********************x기준 오른쪽 회전**********************/
void right_rotate(rbtree *t, node_t *x) {//left의 대칭
  node_t* y = x->left; 
  x->left = y->right; 

  if (y->right != t->nil){
    y->right->parent = x;
  }

  y->parent = x->parent;

  if (x->parent == t->nil){
    t->root = y;
  }else if (x == x->parent->right){
    x->parent->right = y;
  }else {
    x->parent->left = y;
  }
  y->right = x;
  x->parent = y;
  // TODO: 오른쪽 회전 구현
}
/********************x기준 오른쪽 회전**********************/


/****************트리와 관련된 모든 메모리를 해제*****************/
void delete_rbtree(rbtree *t) {
  if (t == NULL) return;//

  //후위 순회 방식으로, 자식 노드를 먼저 삭제한 뒤 현재 노드를 삭제
  void free_node(node_t *node, node_t *nil){
    if (node == nil) return;// 종료 조건은 nil 노드에 도달하면 반환
    free_node(node->left, nil);//왼쪽 서브트리 삭제
    free_node(node->right, nil);//오른쪽 서브트리 삭제
    free(node);//현재 노드 삭제
  }

  free_node(t->root, t->nil); // 모든 메모리 해제
  free(t->nil);               // nil해제
  free(t);                    // 구조체 해제
}
/****************트리와 관련된 모든 메모리를 해제*****************/

/**********************************삽입 후 속성에 맞게 수정****************************************/
void rbtree_insert_fixup(rbtree* t, node_t* z){
  //z는 RED 
  while (z->parent->color == RBTREE_RED){//부모가 빨간색
    if (z->parent == z->parent->parent->left){//부모가 조부모의 왼쪽인 경우
      node_t* y = z->parent->parent->right;//y는 조부모의 오른쪽 즉, 부모의 형제 삼촌 노드 
      //케이스1
      if (y->color == RBTREE_RED) {//삼촌도 빨간색인 경우우
        z->parent->color = RBTREE_BLACK;//부모 검은색으로 변경
        y->color = RBTREE_BLACK;//삼촌도 검은색으로 변경 
        z->parent->parent->color = RBTREE_RED;//조부모는 빨간색으로 변경 
        z = z->parent->parent;//새로 삽입 된 노드는 항상 리프 노드에서 시작함. 
        //그러므로 밑에서 부터 위로 검사를 하기 때문에, 케이스1 색 변환이 끝난 뒤 조부모로 올라가서 그 위 검사 시작
        //케이스 1

        //케이스2
        }else {//부모는 빨강 삼촌은 검정
        if (z == z->parent->right) {//삽입된 레드 노드가 부모의 오른쪽 자식일때 
            z = z->parent;//z를 z의 부모노드로 업데이트 하고 
            left_rotate(t, z);//부모를 기준으로 왼쪽으로 회전
            }
      //케이스2

      z->parent-> color = RBTREE_BLACK;//그 후 부모를 검정으로 바꾸고
      z->parent->parent->color = RBTREE_RED;//조부모도 검정으로 바꾼 뒤
      right_rotate(t, z->parent->parent);//조부모를 기준으로 오른쪽 회전 
  
        }
      }else { // 부모가 조부모의 오른쪽 자식인 경우 (대칭적인 처리)
        node_t *y = z->parent->parent->left; // 삼촌 노드
        if (y->color == RBTREE_RED) { // 케이스 1: 삼촌이 빨간색
          z->parent->color = RBTREE_BLACK; // 부모를 검정색으로
          y->color = RBTREE_BLACK; // 삼촌을 검정색으로
          z->parent->parent->color = RBTREE_RED; // 조부모를 빨간색으로
          z = z->parent->parent; // 조부모로 이동
        } else { // 삼촌이 검정색
          if (z == z->parent->left) { // 케이스 2: z가 부모의 왼쪽 자식인 경우
            z = z->parent;
            right_rotate(t, z); // 오른쪽 회전
          }
          // 케이스 3: z가 부모의 오른쪽 자식인 경우
          z->parent->color = RBTREE_BLACK; // 부모를 검정색으로
          z->parent->parent->color = RBTREE_RED; // 조부모를 빨간색으로
          left_rotate(t, z->parent->parent); // 왼쪽 회전
        }
      }
    }
    t->root->color =RBTREE_BLACK;//루트는 무조건 검은색
  }
/**********************************삽입 후 속성에 맞게 수정****************************************/


/*********************************************삽입**************************************************/
node_t *rbtree_insert(rbtree *t, const key_t key) {
/*************새 노드 초기화**************/
  node_t* z = (node_t *)malloc(sizeof(node_t)); //새 노드 z 생성
  z->key = key; // 새 노드에 키 값 설정
  
  //parent를 지금 초기화 하지 않은 이유는 삽입 위치를 탐색한 후에야 부모 노드가 결정 되기 때문이고 탐색 후 반드시 설정되므로, 초기화 불필요 
/*************새 노드 초기화**************/
  
  /***********삽입 자리를 탐색하기 위한 초기화************/
  node_t* y = t->nil;//포인터 y는 새로 삽입할 노드 z의 부모 노드를 가리키는 포인터로 nil로 초기화 해놓음
  node_t* x = t->root;//포인터 x는 현재 탐색중인 노드, 초기값으로 루트 노드를 가리키는 것으로 하여 탐색 시작 지점 설정
  /***********삽입 자리를 탐색하기 위한 초기화************/
  
  /**************탐색 과정******************/
  // 트리에서 삽입 위치를 찾는 과정
  while(x != t->nil){/*x는 현재 노드, 현재 노드가 nil이 아니라는 것은
    아직 탐색이 끝나지 않았다는 의미, nil노드에 도달할 때까지 탐색을 하기위한 조건 */
    y = x;// 부모 노드를 현재 노드로 업데이트, y는 탐색 중 항상 x의 이전 위치를 저장함.
    
    if(z->key < x->key) { //새로 삽입한 z의 값이 현재 노드인 x보다 작은 경우
      x = x->left;//왼쪽 서브트리로 이동
    } else{//z의 값이 x보다 큰 경우
      x = x->right;//오른쪽 서브트리로 이동
    }
  }
   /**************탐색 과정******************/
  
  /***********새 노드 z 연결 과정************/
  z->parent = y; // 새 노드 z의 부모를 탐색 과정에서 추적한 y로 설정
  
  // 부모 노드와 새 노드를 연결
  if (y == t->nil) {//부모 노드가 nil 노드와 같다는 것은 현재 트리가 비어있다는 뜻
    t->root = z; // 트리가 비어 있으면 새 노드 z를 루트로 설정
  } else if (z->key < y->key) {//트리가 비어 있지 않은 경우, 새 노드 z가 보모 노드인 y보다 작은 경우
    y->left = z; // 새 노드 z를 부모 노드 y의 왼쪽 자식으로 설정
  } else {//새 노드 z가 부모 노드인 y보다 큰 경우
    y->right = z; // 새 노드 z를 부모 노드 y의 오른쪽 자식으로 설정
  }
  /***********새 노드 z 연결 과정************/
  
  z->color = RBTREE_RED;// 새 노드는 항상 빨간색으로 삽입
  z->left = t->nil;// 새 노드의 왼쪽 자식은 NIL 노드
  z->right = t->nil; // 새 노드의 오른쪽 자식은 NIL 노드

  // 삽입 후 레드-블랙 트리의 속성을 복구
  // 새 노드가 계속 red이면 속성에 위반이 되기 때문에 수정이 필요함.
  rbtree_insert_fixup(t, z);
  
  // TODO: 삽입 구현
  return t->root;
}
/*********************************************삽입**************************************************/

/*********************특정 키를 가진 노드 검색********************/
node_t *rbtree_find(const rbtree *t, const key_t key) {//함수 선언부에 트리와 key선언
  node_t *current = t->root;//루트부터 탐색을 하기위해 현재를 나타내는 포인터 변수인 current를 rbtree에 선언

  while (current != t->nil) {//트리의 nil이 나올때까지 검사
    if (key == current->key) {//키를 찾은 경우, 현재를 나타내는 current와 찾는 key의 값이 같은 경우
      return current; // 키를 찾았으므로 현재를 반환 
    } else if (key < current->key) {//현재의 값이 찾는 key값 보다 큰 경우 찾는 값이 더 작다는 뜻이므로
      current = current->left; // 왼쪽 서브트리로 이동
    } else {//현재의 값(28)이 찾는 key(30)값 보다 작은 경우, 괄호 안에 숫자는 예시 
      current = current->right; // 오른쪽 서브트리로 이동
    }
  }

  return NULL; // 키를 찾지 못하면 NULL값 반환
}
/*********************특정 키를 가진 노드 검색********************/


/*********두 자식 노드를 가진 노드를 삭제할 때 대체할 전임자를 찾기 위한 함수*********/
node_t *rbtree_min(const rbtree *t) {
  node_t *current = t->root; // 루트부터 탐색 시작

  // 최소값을 찾는 과정
  while (current != t->nil && current->left != t->nil) {//전임자를 찾기 위해 왼쪽으로 탐색색
    current = current->left;
  }

  return current;
}
/*********두 자식 노드를 가진 노드를 삭제할 때 대체할 전임자를 찾기 위한 함수*********/


/*********두 자식 노드를 가진 노드를 삭제할 때 대체할 후임자를 찾기 위한 함수*********/
node_t *rbtree_max(const rbtree *t) {
  node_t *current = t->root; // 루트부터 탐색 시작

  // 최대값을 찾는 과정
  while (current != t->nil && current->right != t->nil) {
    current = current->right;
  }

  return current;
}
/*********두 자식 노드를 가진 노드를 삭제할 때 대체할 후임자를 찾기 위한 함수*********/


/**********************************삭제 후 속성에 맞게 수정****************************************/
void delete_fixup(rbtree *t, node_t *x) {//x는 속성 위반 가능성이 있는 삭제 된 노드의 자리를 대체한 노드  
  while (x != t->root && x->color == RBTREE_BLACK) {//대체한 노드가 루트가 아니고 블랙일때 
    if (x == x->parent->left) { // x가 왼쪽 자식인 경우
      node_t *w = x->parent->right; // x의 형제 노드는 w 
      if (w->color == RBTREE_RED) { // 케이스 1: 형제가 빨간색
        w->color = RBTREE_BLACK;//형제의 색을 블랙으로 
        x->parent->color = RBTREE_RED;//부모의 색을 레드로 
        left_rotate(t, x->parent);//부모 기준으로 왼쪽 회전 
        w = x->parent->right;//회전 후 새로운 형제 노드를 가져옴 
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { // 케이스 2: 형제의 두 자식이 모두 검정색
        w->color = RBTREE_RED;//형제의 색을 레드로 변경
        x = x->parent;//현재 노드 x를 부모로 변경
      } else {
        if (w->right->color == RBTREE_BLACK) { // 케이스 3: 형제의 오른쪽 자식이 검정색, 왼쪽 자식이 빨간색, 꺾인 경우
          w->left->color = RBTREE_BLACK;//w의 왼쪽 자식을 블랙으로
          w->color = RBTREE_RED;//부모인 w의 색은 레드로
          right_rotate(t, w);//w를 기준으로 오른쪽 회전 해서 케이스 4의 형태로 만듬 
          w = x->parent->right;//x의 부모의 오른쪽 자식을 w로 유지
        }
        // 케이스 4: 형제의 오른쪽 자식이 빨간색
        w->color = x->parent->color;//형제의 색을 조부모의 색으로 변경 
        x->parent->color = RBTREE_BLACK;//조부모는 블랙 
        w->right->color = RBTREE_BLACK;//형제의 오른쪽 자식도 블랙
        left_rotate(t, x->parent);//조부모를 기준으로 왼쪽 회전 
        x = t->root;//함수의 마지막 단계에서 포인터x를 루트를 가리키게 설정
      }
    } else { // x가 오른쪽 자식인 경우 (대칭적인 처리)
      node_t *w = x->parent->left; // x의 형제 노드
      if (w->color == RBTREE_RED) { // 케이스 1: 형제가 빨간색
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) { // 케이스 2: 형제의 두 자식이 모두 검정색
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        if (w->left->color == RBTREE_BLACK) { // 케이스 3: 형제의 왼쪽 자식이 검정색, 오른쪽 자식이 빨간색
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        // 케이스 4: 형제의 왼쪽 자식이 빨간색
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;//x는 루트가 되었으므로 블랙으로 만들어줌 
}
/**********************************삭제 후 속성에 맞게 수정****************************************/


/*********************************************삭제**************************************************/
int rbtree_erase(rbtree *t, node_t *p) {
  
  node_t *y = p; // 삭제할 노드, 두 자식의 경우 y는 후임자가 됨 
  node_t *x;     // 삭제 후 이동할 노드
  color_t y_original_color = y->color; // 삭제 전 y의 색상 저장

  if (p->left == t->nil) { // 왼쪽 자식이 없는 경우
    x = p->right;
    rbtree_transplant(t, p, p->right);//오른쪽 자식을 p의 위치로 대체
  } else if (p->right == t->nil) { // 오른쪽 자식이 없는 경우
    x = p->left;
    rbtree_transplant(t, p, p->left);
  } else { // 두 자식이 모두 있는 경우, 후임자 찾기 
    // 오른쪽 서브트리에서 최소값 찾기 (rbtree_min 호출 대신 직접 탐색)
    y = p->right;//오른쪽에서
    while (y->left != t->nil) {
      y = y->left;//가장 작은 값
    }

    y_original_color = y->color;//삭제 전 원래 색 저장 
    x = y->right;//y의 오른쪽 자식을 x로 설정 

    if (y->parent == p) {//후임자y 가 삭제할 노드p의 자식인 경우 
      x->parent = y;//후임자y 의 오른쪽 자식x의 부모를 후임자 y로 설정 
    } else {//후임자가 삭제할 노드의 오른쪽 자식이 아닌 경우

      rbtree_transplant(t, y, y->right);//후임자의 오른쪽 자식을 후임자의 위치로
      //삭제 할 노드의 오른쪽 자식을 후임자와 연결하는 작업
      y->right = p->right;//후임자의 오른쪽 자식을 삭제 노드의 오른쪽 자식으로 변경 
      y->right->parent = y;//후임자의 오른쪽 자식의 부모는 후임자 
    }

    rbtree_transplant(t, p, y);//후임자를 삭제 노드 위치로 
    y->left = p->left;// 후임자의 왼쪽 자식을 삭제할 노드의 왼쪽 자식으로 설정
    y->left->parent = y; // 삭제할 노드의 왼쪽 자식의 부모를 후임자로 설정
    y->color = p->color;// 후임자의 색상을 삭제할 노드의 색상으로 설정
  }

  free(p); // 삭제된 노드의 메모리 해제

  if (y_original_color == RBTREE_BLACK) {//후임자의 색이 검정인 경우 
    delete_fixup(t, x); // 삭제 후 트리 균형 복구
  }

  return 0;
}
/*********************************************삭제**************************************************/

/***************************중위 순회*******************************/
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {//arr은 데이터를 저장할 배열, n은 배열의 크기
  size_t index = 0;//

  void inorder_traversal(node_t *node, node_t *nil, key_t *arr, size_t *index) {
    if (node == nil || *index >= n) return;
    inorder_traversal(node->left, nil, arr, index);
    arr[(*index)++] = node->key; // 현재 노드의 키를 배열에 저장
    inorder_traversal(node->right, nil, arr, index);
  }

  inorder_traversal(t->root, t->nil, arr, &index);
  return (int)index; // 배열에 저장된 요소의 개수 반환
}
/***************************중위 순회*******************************/