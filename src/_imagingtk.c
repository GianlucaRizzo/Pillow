/*
 * The Python Imaging Library.
 *
 * tkinter hooks
 *
 * history:
 * 99-07-26 fl created
 * 99-08-15 fl moved to its own support module
 *
 * Copyright (c) Secret Labs AB 1999.
 *
 * See the README file for information on usage and redistribution.
 */

#include "Python.h"
#include "hpy.h"

#include "libImaging/Imaging.h"

#include "Tk/_tkmini.h"

/* must link with Tk/tkImaging.c */
extern void
TkImaging_Init(Tcl_Interp *interp);
extern int
load_tkinter_funcs(void);

/* copied from _tkinter.c (this isn't as bad as it may seem: for new
   versions, we use _tkinter's interpaddr hook instead, and all older
   versions use this structure layout) */

typedef struct {
    PyObject_HEAD Tcl_Interp *interp;
} TkappObject;

HPyDef_METH(tkinit, "tkinit", tkinit_impl, HPyFunc_VARARGS)
static HPy tkinit_impl(HPyContext *ctx, HPy self, HPy *args, HPy_ssize_t nargs) {
    HPy h_interp;
    Tcl_Interp *interp;

    HPy arg;
    int is_interp;
    if (!HPyArg_Parse(ctx, NULL, args, nargs, "Oi", &arg, &is_interp)) { 
        return HPy_NULL;
    }

    if (is_interp) {
        interp = (Tcl_Interp *)PyLong_AsVoidPtr(HPy_AsPyObject(ctx, arg));
    } else {
        //HPy h_app;
        TkappObject *app;
        /* Do it the hard way.  This will break if the TkappObject
        layout changes */
        app = (TkappObject *)PyLong_AsVoidPtr(HPy_AsPyObject(ctx, arg));
        interp = app->interp;
    }

    /* This will bomb if interp is invalid... */
    TkImaging_Init(interp);

    return HPy_Dup(ctx, ctx->h_None);
}

static HPyDef *module_defines[] = {
    /* Tkinter interface stuff */
    &tkinit,
    NULL,/* sentinel */
};

HPy_MODINIT(_imagingtk)
static HPy init__imagingtk_impl(HPyContext *ctx) {
    
    static HPyModuleDef module_def = {
        .name = "_imagingtk", /* m_name */
        .doc = NULL,         /* m_doc */
        .size = -1,           /* m_size */
        .defines = module_defines,    /* m_methods */
    };

    HPy h_module = HPyModule_Create(ctx, &module_def);
    if(HPy_IsNull(h_module))
        return HPy_NULL;
    
    return (load_tkinter_funcs() == 0) ? h_module : HPy_NULL;
}
