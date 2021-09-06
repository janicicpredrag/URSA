#ifndef __URSA_H
#define __URSA_H

typedef enum { typeIntConst, typeBoolConst, typeId, typeOpr } nodeEnum;

/* integer constants */
typedef struct {
    char* value;                  /* value of constant */
} intConstNodeType;

/* boolean constants */
typedef struct {
    bool value;                  /* value of constant */
} boolConstNodeType;

/* identifiers */
typedef struct {
    char* i;                    /* var name */
} idNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag *op[1];  /* operands (expandable) */
} oprNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        intConstNodeType intConst;   /* integer constants */
        boolConstNodeType boolConst; /* bool constants */
        idNodeType id;             /* identifiers */
        oprNodeType opr;           /* operators */
    };
} nodeType;


#endif
