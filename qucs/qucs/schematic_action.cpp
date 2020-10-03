/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// user interaction, undo commands
// derived from various Qucs "MouseAction" etc.

#include <QAction>
#include <QMessageBox> // BUG
#include <QLineEdit> // BUG?
#include <QRegExp> // BUG?
#include <QRegExpValidator>
#include <QFileDialog>
#include <QUndoCommand>
#include <QGraphicsSceneEvent>

#include "schematic_doc.h"
#include "qucs.h"
#include "misc.h"
#include "globals.h"
#include "schematic_action.h"
#include "globals.h"

#include "changedialog.h"
#include "components/component.h" // BUG

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include "action/zoom.cpp" // for now.
/*--------------------------------------------------------------------------*/
#include "action/wire.cpp" // for now.
/*--------------------------------------------------------------------------*/
class MouseActionSelect : public MouseAction{
public:
	explicit MouseActionSelect(MouseActions& ctx)
		: MouseAction(ctx), focusElement(nullptr) {}

private: // override
//	cmd* activate(QAction* sender) override;
	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* release(QMouseEvent*) override;
	//	cmd* release2(QMouseEvent*); // what is this?
	// cmd* enter(QEvent*) override;
	cmd* dblclk(QEvent*) override;

#if 0
private: // rectangles?  // this was in MouseActions. BUG. remove
	int MAx1;
	int MAy1;
	int MAx2;
	int MAy2;
#endif
private:
	cmd* release_left(QMouseEvent*);

protected:
	void setPos1(QPointF pos){ untested();
		_pos1 = pos;
	}
	QPointF const& pos1() const{ untested();
		return _pos1;
	}

private: // more decoupling
	ElementMouseAction focusElement;
	bool isMoveEqual; //?
	QPointF _pos1;
};
/*--------------------------------------------------------------------------*/

//void MouseActions::MDoubleClickSelect(SchematicDoc *Doc, QMouseEvent *Event)
QUndoCommand* MouseActionSelect::dblclk(QEvent* evt)
{ untested();
	incomplete();
	//  Doc->releaseKeyboard();  // allow keyboard inputs again
	//  QucsMain->editText->setHidden(true);
	//  editElement(Doc, Event);
	Element* elt = nullptr;
	//
	if(auto i = dynamic_cast<ItemEvent*>(evt)){ untested();
		// QList<ElementGraphics*> l;
		elt = element(&i->item());
		// l.push_back(&i->item());
	}else{ untested();
	}

	if(elt){ untested();
		elt->editElement(&doc());
	}else{ untested();
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// stange glue that should perhaps go into a class derived from QAction and
// replace the button in the toolbar.
template<class CMD>
class MouseActionSelCmd : public MouseAction{
public:
	explicit MouseActionSelCmd(MouseActions& ctx)
		: MouseAction(ctx){}

private:
	cmd* activate(QObject* sender) override;
	cmd* deactivate() override;
//	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* release(QMouseEvent*) override;
//	cmd* generic(QEvent*) override;
private:
	QCursor _oldcursor;
};
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::deactivate()
{itested();
	setCursor(_oldcursor);
	return MouseAction::deactivate();
}
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::activate(QObject* sender)
{itested();
	MouseAction::activate(sender); // ...

	_oldcursor = doc().cursor();
	setCursor(Qt::CrossCursor);

	auto s = doc().selectedItems();
	bool selected = !s.empty();

	if(selected){itested();
		auto cmd = new CMD(doc(), s);
		return cmd;
	}else{ untested();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::press(QEvent* e)
{ untested();
	if(!e){ untested();
		return nullptr;
	}else if(auto i = dynamic_cast<ItemEvent*>(e)){ untested();
		QList<ElementGraphics*> l;
		l.push_back(&i->item());
		return new CMD(doc(), l);
	}else{ untested();
		trace1("delete::scene unknown sender", e->type());
		return nullptr;
	}
} // delete::scene
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::release(QMouseEvent*)
{ untested();
	incomplete(); // why?
	return nullptr;
} // delete::release
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class DeleteSelection : public QUndoCommand {
public:
	template<class IT>
	DeleteSelection(SchematicDoc& ctx, IT selection)
	: _ctx(ctx), _done(false){ untested();
		size_t k = 0;
		for(auto i : selection){ untested();
			++k;
			if(auto eg=dynamic_cast<ElementGraphics*>(i)){ untested();
				_gfx.push_back(eg);
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}
		setText("delete " + QString::number(k) + " items");
	}
	void undo() override { untested();
		QUndoCommand::undo(); // does not check

		assert(_done);
		for(auto& d : _gfx){ untested();
			d->show();
		}
		_done = false;
	}
	void redo() override { untested();
		QUndoCommand::redo(); // does not check

		assert(!_done);
		for(auto& d : _gfx){ untested();
			d->hide();
		}
		_done = true;
	}
private:
    SchematicDoc& _ctx;
    std::vector<ElementGraphics*> _gfx;
	 bool _done;
}; // DeleteSelection
typedef MouseActionSelCmd<DeleteSelection> MouseActionDelete;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class T>
class TransformSelection : public QUndoCommand {
public:
	template<class IT>
	TransformSelection(SchematicDoc& ctx, IT selection, T const& transform)
	: _ctx(ctx), _t(transform){itested();
		size_t k = 0;
		// TODO: bounding box center?
		QRectF bb;
		for(auto i : selection){itested();
			++k;
			QGraphicsItem const* g = i;//huh?
			if(auto eg=dynamic_cast<ElementGraphics*>(i)){itested();
				auto br = g->boundingRect();
				br.translate(eg->pos());
				bb |= br;
				trace1("DBG", bb.center());
				_gfx.push_back(eg);
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}
		_pivot_g.first = getX(bb.center());
		_pivot_g.second = getY(bb.center());

		setText("transform " + QString::number(k) + " items");
	}
	void undo() override { untested();
		auto t = _t.inverse();
		for(auto& d : _gfx){ untested();
			d->transform(t, _pivot_g);
		}
	}
	void redo() override {itested();
		for(auto& d : _gfx){itested();
			d->transform(_t, _pivot_g);
		}
	}
private:
    SchematicDoc& _ctx;
    std::vector<ElementGraphics*> _gfx;
	 std::pair<int, int> _pivot_g; // pivot in global coordinates
	 T const& _t;
}; // TransformSelection
/*--------------------------------------------------------------------------*/
static const rotate_after_mirror1_t ninety_degree_transform(270, false); // !!
class RotateSelectionTransform : public TransformSelection<qucsSymbolTransform>{
	typedef TransformSelection<qucsSymbolTransform> base;
public:
	template<class IT>
	RotateSelectionTransform(SchematicDoc& ctx, IT selection)
	  : base(ctx, selection, ninety_degree_transform) {}
};
/*--------------------------------------------------------------------------*/
class MirrorXaxisSelection : public TransformSelection<qucsSymbolTransform>{
	typedef TransformSelection<qucsSymbolTransform> base;
public:
	template<class IT>
	MirrorXaxisSelection(SchematicDoc& ctx, IT selection)
	  : base(ctx, selection, mirrorXaxis) {}
};
/*--------------------------------------------------------------------------*/
class MirrorYaxisSelection : public TransformSelection<qucsSymbolTransform>{
	typedef TransformSelection<qucsSymbolTransform> base;
public:
	template<class IT>
	MirrorYaxisSelection(SchematicDoc& ctx, IT selection)
	  : base(ctx, selection, mirrorYaxis) {}
};
/*--------------------------------------------------------------------------*/
typedef MouseActionSelCmd<DeleteSelection> MouseActionDelete;
typedef MouseActionSelCmd<DeleteSelection> MouseActionActivate; // TODO
typedef MouseActionSelCmd<RotateSelectionTransform> MouseActionRotate;
typedef MouseActionSelCmd<MirrorXaxisSelection> MouseActionMirrorXaxis;
typedef MouseActionSelCmd<MirrorYaxisSelection> MouseActionMirrorYaxis;
/*--------------------------------------------------------------------------*/
class MouseActionNewElement : public MouseAction{
public:
	explicit MouseActionNewElement(MouseActions& ctx, Element const* proto=nullptr)
		: MouseAction(ctx), _gfx(nullptr), _proto(proto)
  	{}
private:
	cmd* activate(QObject* sender) override;
	cmd* deactivate() override;
	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* enter(QEvent*) override;
	cmd* leave(QEvent*) override;
	cmd* release(QMouseEvent*) override;

private:
	cmd* makeNew(QMouseEvent*);
	cmd* rotate(QEvent*);

private:
	ElementGraphics* _gfx;
	Element const* _proto;
};
/*--------------------------------------------------------------------------*/
class NewElementCommand : public QUndoCommand {
public:
	NewElementCommand(SchematicDoc& ctx, ElementGraphics* gfx)
	: _ctx(ctx), _gfx(gfx){ untested();
		ctx.takeOwnership(element(gfx)); // BUG?
		// elment->setOwner(ctx)...?
		setText("NewElement" /*element(gfx)->label()*/); // tr?
		trace0("NewElementCommand::NewElementCommand");
	}
	~NewElementCommand(){ untested();
		// _gfx is owned by ctx
	}
public:
	void undo() override { untested();
		QUndoCommand::undo();
		_gfx->hide();
		_done = false;
	}
	void redo() override { untested();
		QUndoCommand::redo();
		trace0("NewElementCommand::redo");
		_gfx->show();
		_done = true;
	}
private:
    SchematicDoc& _ctx;
    ElementGraphics* _gfx;
	 bool _done;
}; // NewElementCommand
/*--------------------------------------------------------------------------*/
#include <component_widget.h> // not here.
QUndoCommand* MouseActionNewElement::activate(QObject* sender)
{
	if(auto s=dynamic_cast<ComponentListWidgetItem*>(sender)){
		_proto = s->proto(); // better clone?
	}else{
	}
	return MouseAction::activate(sender);
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::release(QMouseEvent* ev)
{ untested();
	QUndoCommand* cmd = nullptr;
	auto m = dynamic_cast<QMouseEvent*>(ev);
	if(!m){ untested();
	}else if(m->button() == Qt::LeftButton){ untested();
		cmd = makeNew(ev);
	}else if(m->button() == Qt::RightButton){ untested();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::makeNew(QMouseEvent* ev)
{ untested();
	// assert(ev->widget=doc->scene()) // or so.
	trace1("RELEASE", ev->type());
	if(ev->type() == QEvent::MouseButtonRelease){ itested();
	}else{ untested();
		unreachable();
	}

	assert(element(_gfx));
	auto elt = element(_gfx);
	assert(elt);

	if(auto se=dynamic_cast<QGraphicsSceneMouseEvent*>(ev)){ untested();
		QPointF pos = se->scenePos();
		QPoint xx = doc().snapToGrid(pos);
		_gfx->setPos(xx);
	}else{
	}

	cmd* c = new NewElementCommand(doc(), _gfx);
	// _gfx = nullptr;

	{ untested();
		_gfx = _gfx->clone(); // new ElementGraphics(elt);
		doc().sceneAddItem(_gfx); // does not attach.
		assert(!element(_gfx)->scope());
	}

	ev->accept();
	return c;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::deactivate()
{ untested();
	// assert(!attached);
	doc().sceneRemoveItem(_gfx);
	delete _gfx; // CHECK: who owns _elt?
	_gfx = nullptr;
	incomplete();
	return MouseAction::deactivate();
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::move(QEvent* ev)
{ untested();
	QPointF sp;
	trace1("move", ev->type());
	if(auto ee=dynamic_cast<QMouseEvent*>(ev)){ untested();
		unreachable();
		QPointF wp;
		wp = ee->localPos(); // use oldPos?
		sp = mapToScene(wp.toPoint());
	}else if(auto ee=dynamic_cast<QGraphicsSceneMouseEvent*>(ev)){ untested();
		sp = ee->scenePos();

		QPoint xx = doc().snapToGrid(sp);
		sp = xx;
	}else{ untested();
		unreachable();
	}

	if(_gfx){ untested();
		_gfx->setPos(sp.x(), sp.y());
	}else{ untested();
		unreachable();
	}

	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::enter(QEvent* ev)
{ untested();
	trace1("new enter", ev->type());
	auto ee = prechecked_cast<QEnterEvent*>(ev);
	assert(ee);
	
	auto wp = ee->localPos();

	SchematicDoc* d = &doc();
	auto sp = d->mapToScene(wp.toPoint());

	Element* elt;
	if(!_gfx){ untested();
		assert(_proto);
		elt = _proto->clone();
		elt->setCenter(sp.x(), sp.y());
		_gfx = new ElementGraphics(elt); // BUG
	}else{ untested();
		_gfx->setPos(sp.x(), sp.y());
	}
	
	doc().sceneAddItem(_gfx);

	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::leave(QEvent* ev)
{ untested();
	sceneRemoveItem(_gfx);
	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::rotate(QEvent*)
{ untested();
	if(!_gfx){ untested();
		unreachable();
	}else if(dynamic_cast<Symbol*>(element(_gfx))){ untested();
		// always do this?
		_gfx->transform(ninety_degree_transform);
	}else{ untested();
		unreachable();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::press(QEvent* ev)
{ untested();
	auto a = dynamic_cast<QMouseEvent*>(ev);
	auto m = dynamic_cast<QGraphicsSceneMouseEvent*>(ev);
	QUndoCommand* cmd = nullptr;
	if(a){ untested();
		unreachable();
		// somehow it is a scene event??
	}else if(!m){ untested();
		trace1("MouseActionNewElement::press", ev->type());
		unreachable();
	}else if(m->button() == Qt::LeftButton){ untested();
	}else if(m->button() == Qt::RightButton){ untested();
		cmd = MouseActionNewElement::rotate(ev);
		ev->accept(); // really?
	}else{ untested();
		unreachable();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include "action/move.cpp" // for now.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// was: MouseActions::MMoveSelect
QUndoCommand* MouseActionSelect::move(QEvent *)
{itested();
	// obsolete?
	if(isMoveEqual) {itested();
		// square?
	}else{itested();
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
// was: MouseActions::MPressSelect
QUndoCommand* MouseActionSelect::press(QEvent*)
{itested();

	incomplete();
	return nullptr;
#if 0
	QMouseEvent* e;
	SchematicDoc* Doc = &doc();
	assert(Doc);
	QPointF pos = Doc->mapToScene(e->pos());
	trace2("sel::press", pos, e->pos());
	setPos1(pos);
	float fX=pos.x();
	float fY=pos.y();

	bool Ctrl = e->modifiers().testFlag(Qt::ControlModifier);

	int No=0;

	// memorise first click position
	//MAx1 = int(fX);
	//MAy1 = int(fY);

	focusElement = ctx().selectElement(e->pos(), Ctrl, &No);
	isMoveEqual = false;   // moving not neccessarily square

	incomplete(); //this does not add up.
#if 0
	if(!focusElement){ untested();
	}else if(focusElement->Type == isDiagramHScroll){ untested();
		// BUG: move to selectElement? what is MAy1?!
		MAy1 = MAx1;
	}else{ untested();
	}
#endif

	if(!focusElement){ untested();
		qDebug() << "MPressSelect miss" << e->pos() << pos;
	}else if(focusElement->Type == isPaintingResize){ untested();
		incomplete(); // delegate. how?
#if 0
		focusElement->Type = isPainting;
		QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizePainting;
		QucsMain->MouseMoveAction = &MouseActions::MMoveResizePainting;
		QucsMain->MousePressAction = 0;
		QucsMain->MouseDoubleClickAction = 0;
		Doc->grabKeyboard();  // no keyboard inputs during move actions
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return nullptr;
	}else if(focusElement->Type == isDiagramResize){ untested();
		incomplete();

		if(((Diagram*)focusElement)->name().left(4) != "Rect")
			if(((Diagram*)focusElement)->name().at(0) != 'T')
				if(((Diagram*)focusElement)->name() != "Curve")
					/* if(((Diagram*)focusElement)->name() != "Waveac")
						if(((Diagram*)focusElement)->name() != "Phasor")*/
					isMoveEqual = true;  // diagram must be square

		focusElement->Type = isDiagram;
		MAx1 = focusElement->cx_();
		MAx2 = focusElement->x2_();

		focusElement->someDiagramStateCallback()

			// old:
			// if(diagram(focusElement)->State & 1) { untested();
			//   MAx1 += MAx2;
			//   MAx2 *= -1;
			// }

			MAy1 =  focusElement->cy_();
		MAy2 = -focusElement->y2_();
		if(((Diagram*)focusElement)->State & 2) { untested();
			MAy1 += MAy2;
			MAy2 *= -1;
		}

		 // diagram_action?
		QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizeDiagram;
		QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
		QucsMain->MousePressAction = 0;
		QucsMain->MouseDoubleClickAction = 0;
		Doc->grabKeyboard(); // no keyboard inputs during move actions
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return nullptr;

	}else if(focusElement->Type == isDiagramHScroll
			|| focusElement->Type == isDiagramVScroll){ untested();
		incomplete();

		focusElement->Type = isDiagram; // reset happens here. FIXME.

		auto d=diagram(focusElement); // is this necessary?!
		assert(d);
		No = d->scroll(MAy1);

		switch(No) {
		case 1:
			Doc->setChanged(true, true, 'm'); // 'm' = only the first time
			break;
		case 2:  // move scroll bar with mouse cursor
			QucsMain->MouseMoveAction = &MouseActions::MMoveScrollBar;
			QucsMain->MousePressAction = 0;
			QucsMain->MouseDoubleClickAction = 0;
			Doc->grabKeyboard();  // no keyboard inputs during move actions

			// Remember inital scroll bar position.
			MAx2 = int(d->xAxis_limit_min());
			// Update matching wire label highlighting
			Doc->highlightWireLabels ();
			return;
		}
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		Doc->viewport()->update();
		drawn = false;
		return;

	}else if(focusElement->Type == isComponentText){ untested();
		incomplete();

		focusElement->Type &= (~isComponentText) | isComponent;

		MAx3 = No;
		QucsMain->slotApplyCompText();
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return;

	}else if(auto n=node(focusElement)){ untested();
		(void)n;
		if (QucsSettings.NodeWiring) { untested();
			incomplete();

			MAx1 = 0;   // paint wire corner first up, then left/right
			MAx3 = focusElement->cx_();  // works even if node is not on grid
			MAy3 = focusElement->cy_();
			QucsMain->MouseMoveAction = &MouseActions::MMoveWire2;
			QucsMain->MousePressAction = &MouseActions::MPressWire2;
			QucsMain->MouseReleaseAction = 0; // if function is called from elsewhere
			QucsMain->MouseDoubleClickAction = 0;

			formerAction = QucsMain->select; // to restore action afterwards
			QucsMain->activeAction = QucsMain->insWire;

			QucsMain->select->blockSignals(true);
			QucsMain->select->setChecked(false);
			QucsMain->select->blockSignals(false);

			QucsMain->insWire->blockSignals(true);
			QucsMain->insWire->setChecked(true);
			QucsMain->insWire->blockSignals(false);
			// Update matching wire label highlighting
			Doc->highlightWireLabels ();
			return;
		}else{ untested();
		}
#endif
	}else{ untested();
		// default case
		// unreachable?
	}

	QucsMain->MousePressAction = 0;
	QucsMain->MouseDoubleClickAction = 0;
	Doc->grabKeyboard();  // no keyboard inputs during move actions
	Doc->viewport()->update();
	//setDrawn(false);

#if 0 // obsolete
	if(!focusElement) { untested();
		unreachable();
		// rectangleselect obsolete.
		MAx2 = 0;  // if not clicking on an element => open a rectangle
		MAy2 = 0;
		// QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect2;
		// QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
		//state = drag_rectangle;
	}else{ untested();
		// element could be moved
		if(Ctrl) { untested();
		}else{ untested();
			if(!focusElement->isSelected()) { untested();
				// Don't move selected elements if clicked
				// deselectElements(focusElement); // element was not selected.
			}else{ untested();
			}
			//focusElement->setSelected();
		}
		Doc->setOnGrid(MAx1, MAy1);

//    it seems move is done by QT scene.
//		QucsMain->MouseMoveAction = &MouseActions::MMoveMoving;
	}
#endif
	// Update matching wire label highlighting
	assert(Doc);
	// Doc->highlightWireLabels ();
//	e->ignore(); // handle in QGraphicsView?
	return nullptr;
#endif
} // select::press
/*--------------------------------------------------------------------------*/
// was MouseActions::MReleaseSelect(SchematicDoc *Doc, QMouseEvent *Event)
QUndoCommand* MouseActionSelect::release(QMouseEvent *ev)
{itested();
	QUndoCommand* cmd = nullptr;
	auto m = dynamic_cast<QMouseEvent*>(ev);
	if(!m){ untested();
	}else if(m->button() == Qt::LeftButton){itested();
		cmd = release_left(ev);
	}else if(m->button() == Qt::RightButton){ untested();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
static QPoint getDelta(ElementGraphics* e)
{ untested();
	auto p = e->pos().toPoint();
	assert(element(e));
	auto p1_ = element(e)->center();
	auto p1 = QPoint(getX(p1_), getY(p1_));
	return p - p1;
}
/*--------------------------------------------------------------------------*/
// it's a "wire" if it has two ports that connect to the same net.
static bool isWire(Symbol const* e)
{
	assert(e);
	if(e->numPorts()!=2){
		return false;
	}else if(e->portValue(0) == e->portValue(1)){
		return true;
	}else{
		return false;
	}
}
/*--------------------------------------------------------------------------*/
Symbol* symbol(Element* e)
{
	return dynamic_cast<Symbol*>(e);
}
Symbol* symbol(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return symbol(e->operator->());
}
// Follow a wire line and select it.
/*--------------------------------------------------------------------------*/
// static??
void SchematicDoc::selectWireLine(ElementGraphics *, Node const*, bool /*ctrl*/)
{ untested();
#if 0
	Symbol const* pe = symbol(g);
	Node const* pn_1st = pn;
	while(pn->degree() == 2) {
		if(pn->firstConnection() == pe){
			pe = symbol(pn->lastConnection());
		}else{
			pe = symbol(pn->firstConnection());
		}
		assert(pe);

		if(!isWire(pe)){
			break;
		}else if(ctrl){
			g->setSelected(!g->isSelected());
		} else{ untested();
			g->setSelected(true);
		}

		if(pe->portNode(0) == pn){
		  	pn = pe->portNode(1);
		} else {
			assert(pe->portNode(1) == pn);
			pn = pe->portNode(0);
		}

		if(pn == pn_1st){
		  	break;  // avoid endless loop in wire loops
		}else{
		}
	}
#endif
}

QPointF makeQPointF(std::pair<int,int> p)
{
	return QPointF(p.first, p.second);
}
/*--------------------------------------------------------------------------*/
static void selectWireLine(ElementGraphics *g)
{
	Symbol* s = symbol(g);
	assert(isWire(s));
	auto scn = g->scene();
	assert(scn);
	
	for(unsigned i=0; i<s->numPorts(); ++i){
		auto pos = makeQPointF(s->portPosition(i));
		auto items = scn->items(pos);
		if(items.size()==2){

			for(auto ii : scn->items(pos)){
				Symbol* si = symbol(ii);
				if(!si){
				}else if(g->isSelected()==ii->isSelected()){
				}else if(si == s){
				}else if(isWire(si)){
					ii->setSelected(g->isSelected());
					selectWireLine(ii);
				}else{
				}
			}
		}
	}
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionSelect::release_left(QMouseEvent *Event)
{untested();
	bool ctrl = Event->modifiers().testFlag(Qt::ControlModifier);

	if(!ctrl) {itested();
		incomplete();
	}else{ untested();
	}

	cmd* c = nullptr;

	auto s = doc().selectedItems();
	if(s.isEmpty()){ untested();
	}else{itested();

		auto delta = getDelta(s.first());
#ifndef NDEBUG
		for(auto i : s){
			assert(delta == getDelta(i));
		}
#endif
		int fX = int(delta.x());
		int fY = int(delta.y());

		if(fX || fY){ untested();
			trace1("possible move", delta);
			c = new MoveSelection(delta, doc(), s);
		}else{itested();
		}
	}

	if(c){
	}else if(s.size()!=1){
	}else if(!symbol(s.front())){
	}else if(Event->button() == Qt::LeftButton){ untested();
			// if it's a wire, select the whole thing?
			// (what is a wire?)
		if(isWire(symbol(s.front()))) { untested();
			incomplete();
#if 1
			selectWireLine(s.front());
#endif
		}else{ untested();
		}
	}else{itested();
	}

	doc().releaseKeyboard();

#if 0
	// is it not in "select" mode already?
	QucsMain->MouseMoveAction = 0;
	QucsMain->MousePressAction = &MouseActions::MPressSelect;
	QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
	QucsMain->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
#endif

//	Doc->highlightWireLabels ();
	updateViewport();
	// drawn = false;
	return c;
} // select::release
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SchematicActions::SchematicActions(SchematicDoc& ctx)
  : MouseActions(ctx)
{itested();

	// not entirely clear how to refactor this
	// maybe dispatch mouse actions.
	// or merge into QAction buttons (connect as needed?)

	maDelete = new MouseActionDelete(*this);
	maSelect = new MouseActionSelect(*this);
	maWire = new MouseActionWire(*this);
	maZoomIn = new MouseActionZoomIn(*this);
//	maZoomOut = new MouseActionZoomOut(*this); // not a mouseaction.

	//  maMove = new MouseActionMove(*this);
	Element const* gnd = symbol_dispatcher["GND"];
	assert(gnd);
	maInsertGround = new MouseActionNewElement(*this, gnd);

	Element const* port = symbol_dispatcher["Port"];
	assert(port);
	maInsertPort = new MouseActionNewElement(*this, port);

	maInsertElement = new MouseActionNewElement(*this);

	maActivate = new MouseActionActivate(*this);
	maMirrorXaxis = new MouseActionMirrorXaxis(*this);
	maMirrorYaxis = new MouseActionMirrorYaxis(*this);
	maRotate = new MouseActionRotate(*this);

	// this was in App previously, and scattered across a couple of pointer hacks.
	// possibly initialised to "select". recheck.
	_maCurrent = maSelect;
}
/*--------------------------------------------------------------------------*/
SchematicActions::~SchematicActions()
{itested();
	delete maActivate;
	delete maDelete;
	delete maInsertGround;
	delete maInsertPort;
	delete maMirrorXaxis;
	delete maMirrorYaxis;
	delete maRotate;
	delete maSelect;
	delete maWire;
	delete maZoomIn;
}
/*--------------------------------------------------------------------------*/
SchematicScene const* SchematicActions::scene() const
{ untested();
	auto d=dynamic_cast<SchematicDoc const*>(&_doc);
	assert(d);
	return d->scene();
}
/*--------------------------------------------------------------------------*/
SchematicDoc* SchematicActions::doc()
{ untested();
	auto d=dynamic_cast<SchematicDoc*>(&_doc);
	assert(d);
	return d;
}
/*--------------------------------------------------------------------------*/
void SchematicActions::updateViewport()
{ untested();
	doc()->updateViewport();	
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

QRegExp Expr_CompProp;
QRegExpValidator Val_CompProp(Expr_CompProp, 0);

// forward to mouseActions...
void SchematicDoc::actionSelect(QAction* sender)
{itested();

#if 0
  // goto to insertWire mode if ESC pressed during wiring
  if(App->MouseMoveAction == &MouseActions::MMoveWire2) { untested();
	  incomplete();
    App->MouseMoveAction = &MouseActions::MMoveWire1;
    App->MousePressAction = &MouseActions::MPressWire1;
    viewport()->update();
    mouseActions().setDrawn(false);


    // see EditDelete
//  }else if(performToggleAction(on, selectAction(), 0, 0, &MouseActions::MPressSelect)) { untested();
//    App->MouseReleaseAction = &MouseActions::MReleaseSelect;
//    App->MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
//
//  }else{ untested();
  }else{ untested();
  }
#endif

  // sender is a button. maSelect is an action. connect the two.
  // this looks a bit redundant
  // if(on)..?
  possiblyToggleAction(schematicActions().maSelect, sender);

} // SchematicDoc::actionSelect

void SchematicDoc::actionOnGrid(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maOnGrid, sender);
//  performToggleAction(on, App->onGrid, &SchematicDoc::elementsOnGrid,
//		&MouseActions::MMoveOnGrid, &MouseActions::MPressOnGrid);
}

void SchematicDoc::actionEditRotate(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maRotate, sender);
//  performToggleAction(on, App->editRotate, &SchematicDoc::rotateElements,
//		&MouseActions::MMoveRotate, &MouseActions::MPressRotate);
}

void SchematicDoc::actionEditMirrorX(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maMirrorYaxis, sender);
//  performToggleAction(on, App->editMirror, &SchematicDoc::mirrorXComponents,
//		&MouseActions::MMoveMirrorX, &MouseActions::MPressMirrorX);
}

void SchematicDoc::actionEditMirrorY(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maMirrorXaxis, sender);
//  performToggleAction(on, App->editMirrorY, &SchematicDoc::mirrorYComponents,
//		&MouseActions::MMoveMirrorY, &MouseActions::MPressMirrorY);
}

void SchematicDoc::actionEditActivate(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maActivate, sender);
}

void SchematicDoc::actionEditDelete(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maDelete, sender);

  updateViewport();
//  assert(mouseActions());
//  mouseActions()->setDrawn(false);
}

void SchematicDoc::actionSetWire(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maWire, sender);
}

void SchematicDoc::actionInsertLabel(QAction*)
{ untested();
//  possiblyToggleAction(schematicActions().maInsertLabel, sender);
  incomplete();
//  performToggleAction(on, App->insLabel, 0,
//		&MouseActions::MMoveLabel, &MouseActions::MPressLabel);
  // mouseAction = mouseActions().maInsLabel;
}

void SchematicDoc::actionSetMarker(QAction*)
{ untested();
  incomplete();
//  performToggleAction(on, App->setMarker, 0,
//		&MouseActions::MMoveMarker, &MouseActions::MPressMarker);
  // mouseAction = mouseActions().maSetMarker;
}

void SchematicDoc::actionMoveText(QAction*)
{ untested();
  incomplete();
//  performToggleAction(on, App->moveText, 0,
//		&MouseActions::MMoveMoveTextB, &MouseActions::MPressMoveText);
  // mouseAction = mouseActions().maMoveText;
}

void SchematicDoc::actionZoomIn(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maZoomIn, sender);
}

void SchematicDoc::actionInsertEquation(QAction*)
{ untested();
	incomplete();
#if 0
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) { untested();
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) { untested();
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setChecked(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = App->insEquation;

  if(mouseActions().hasElem()){ untested();
    incomplete(); // undo??
    delete mouseActions().getElem();  // delete previously selected component
  }else{ untested();
  }

  Symbol* sym=symbol_dispatcher.clone("Eqn");
  assert(sym);
  mouseActions().setElem(prechecked_cast<Component*>(sym));
  assert(mouseActions().hasElem());

  if(mouseActions().wasDrawn()){ untested();
    updateViewport();
  }else{ untested();
  }
  mouseActions().setDrawn(false);
  App->MouseMoveAction = &MouseActions::MMoveElement;
  App->MousePressAction = &MouseActions::MPressElement;
#endif
}

void SchematicDoc::actionEditPaste(QAction*)
{ untested();
#if 0
	// if it's not a text doc, prevent the user from editing
	// while we perform the paste operation
	App->hideEdit();

	if(!on) { untested();
		App->MouseMoveAction = 0;
		App->MousePressAction = 0;
		App->MouseReleaseAction = 0;
		App->MouseDoubleClickAction = 0;
		App->activeAction = 0;   // no action active
		if(mouseActions().wasDrawn()) { untested();
			viewport()->update();
		}
		return;
	}else{ untested();
        }

	if(!mouseActions()->pasteElements(this)) { untested();
		App->editPaste->blockSignals(true); // do not call toggle slot
		App->editPaste->setChecked(false);       // set toolbar button off
		App->editPaste->blockSignals(false);
		return;   // if clipboard empty
	}else{ untested();
        }

	uncheckActive();
	App->activeAction = App->editPaste;

        mouseActions().setDrawn(false)
	App->MouseMoveAction = &MouseActions::MMovePaste;
	mouseActions()->movingRotated = 0;
	App->MousePressAction = 0;
	App->MouseReleaseAction = 0;
	App->MouseDoubleClickAction = 0;

#endif
}

void SchematicDoc::actionSelectElement(QObject*e)
{ untested();
  schematicActions().maInsertElement->activate(e);
  possiblyToggleAction(schematicActions().maInsertElement, nullptr);
}

void SchematicDoc::actionInsertGround(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maInsertGround, sender);
#if 0
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) { untested();
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }

  uncheckActive();
  App->activeAction = App->insGround;

  if(mouseActions().hasElem()){ untested();
    incomplete(); // undo.
//    delete mouseActions()->selElem;  // delete previously selected component
  }

  Symbol* sym=symbol_dispatcher.clone("GND");
  assert(sym);
  mouseActions()->selElem = prechecked_cast<Component*>(sym);
  assert(mouseActions()->selElem);

  if(mouseActions().wasDrawn()){ untested();
    updateViewport();
  }else{ untested();
  }
  mouseActions().setDrawn(false)
  App->MouseMoveAction = &MouseActions::MMoveElement;
  App->MousePressAction = &MouseActions::MPressElement;
#endif
}

void SchematicDoc::actionInsertPort(QAction* sender)
{ untested();
  possiblyToggleAction(schematicActions().maInsertPort, sender);
#if 0
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) { untested();
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) { untested();
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setChecked(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = App->insPort;

  if(mouseActions()->selElem)
    delete mouseActions()->selElem;  // delete previously selected component

  Symbol* sym=symbol_dispatcher.clone("Port");
  assert(sym);
  mouseActions()->selElem = prechecked_cast<Component*>(sym);
  assert(mouseActions()->selElem);

  if(mouseActions()->drawn) viewport()->update();
  mouseActions()->drawn = false;
  App->MouseMoveAction = &MouseActions::MMoveElement;
  App->MousePressAction = &MouseActions::MPressElement;
#endif
}

// is this still in use?
void SchematicDoc::actionEditUndo(QAction*)
{ untested();
  // really?
  App->hideEdit(); // disable text edit of component property

  undo();
  updateViewport();
  assert(mouseActions());
  setDrawn(false);
}

// is this still in use?
void SchematicDoc::actionEditRedo(QAction*)
{ untested();
  App->hideEdit(); // disable text edit of component property

  redo();
  updateViewport();
  setDrawn(false);
}

void SchematicDoc::actionAlign(int what)
{ untested();
  App->hideEdit(); // disable text edit of component property

  if(!aligning(what)){ untested();
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  }
  updateViewport();
  assert(mouseActions());
  setDrawn(false);
}

void SchematicDoc::actionDistrib(int d)
{ untested();
  App->hideEdit(); // disable text edit of component property

  if (d==0){ untested();
	  distributeHorizontal();
  }else if(d==1){ untested();
	  distributeVertical();
  }else{ untested();
	  unreachable();
  }
  updateViewport();
  setDrawn(false);
}

void SchematicDoc::actionSelectAll(QAction*)
{ untested();
    selectElements(INT_MIN, INT_MIN, INT_MAX, INT_MAX, true);
    updateViewport();
}

void SchematicDoc::actionSelectMarker()
{ untested();
  App->hideEdit(); // disable text edit of component property

  selectMarkers();
  updateViewport();
  setDrawn(false);
}

void SchematicDoc::actionChangeProps(QAction*)
{ untested();
	ChangeDialog *d = new ChangeDialog(this);
	if(d->exec() == QDialog::Accepted) { untested();
		setChanged(true, true);
		updateViewport();
	}
}

void SchematicDoc::actionCursor(arrow_dir_t)
{ untested();
	incomplete();
#ifdef USE_SCROLLVIEW
	int sign = 1;

	if(dir==arr_left){ untested();
		sign = -1;
	}

	if(App->editText->isHidden()) { untested();
		// for edit of component property ?
		auto movingElements = cropSelectedElements();
		int markerCount=0;
		for(auto const& i : movingElements){ untested();
			if(marker(i)){ untested();
				++markerCount;
			}
		}

		if((movingElements.count() - markerCount) < 1) { // all selections are markers
			incomplete();
#if 0
			if(markerCount > 0) {  // only move marker if nothing else selected
				markerMove(dir, &movingElements);
			} else if(dir==arr_up) { untested();
				// nothing selected at all
				if(scrollUp(verticalScrollBar()->singleStep()))
					scrollBy(0, -verticalScrollBar()->singleStep());
			} else if(dir==arr_down) { untested();
				if(scrollDown(-verticalScrollBar()->singleStep()))
					scrollBy(0, verticalScrollBar()->singleStep());
			} else if(dir==arr_left) { untested();
				if(scrollLeft(horizontalScrollBar()->singleStep()))
					scrollBy(-horizontalScrollBar()->singleStep(), 0);
			} else if(dir==arr_right) { untested();
				if(scrollRight(-horizontalScrollBar()->singleStep()))
					scrollBy(horizontalScrollBar()->singleStep(), 0);
			}else{ untested();
				// unreachable. TODO: switch
			}

			updateViewport();
			mouseActions()->drawn = false;
#endif
		}else if(dir==arr_up || dir==arr_down){ untested();
			// some random selection, put it back
			mouseActions()->moveElements(movingElements, 0, ((dir==arr_up)?-1:1) * GridY);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else if(dir==arr_left || dir==arr_right){ untested();
			mouseActions()->moveElements(movingElements, sign*GridX, 0);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else{ untested();
			//unreachable(); //TODO: switch.
		}

	}else if(dir==arr_up){ // BUG: redirect.
		if(mouseActions()->MAx3 == 0) return;  // edit component namen ?
		Component *pc = component(mouseActions()->focusElement);
		Property *pp = pc->Props.at(mouseActions()->MAx3-1);  // current property
		int Begin = pp->Description.indexOf('[');
		if(Begin < 0) return;  // no selection list ?
		int End = pp->Description.indexOf(App->editText->text(), Begin); // current
		if(End < 0) return;  // should never happen
		End = pp->Description.lastIndexOf(',', End);
		if(End < Begin) return;  // was first item ?
		End--;
		int Pos = pp->Description.lastIndexOf(',', End);
		if(Pos < Begin) Pos = Begin;   // is first item ?
		Pos++;
		if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
		App->editText->setText(pp->Description.mid(Pos, End-Pos+1));
		App->editText->selectAll();
	}else if(dir==arr_down) { // BUG: redirect.
		if(mouseActions()->MAx3 == 0) return;  // edit component namen ?
		Component *pc = component(mouseActions()->focusElement);
		Property *pp = pc->Props.at(mouseActions()->MAx3-1);  // current property
		int Pos = pp->Description.indexOf('[');
		if(Pos < 0) return;  // no selection list ?
		Pos = pp->Description.indexOf(App->editText->text(), Pos); // current list item
		if(Pos < 0) return;  // should never happen
		Pos = pp->Description.indexOf(',', Pos);
		if(Pos < 0) return;  // was last item ?
		Pos++;
		if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
		int End = pp->Description.indexOf(',', Pos);
		if(End < 0) {  // is last item ?
			End = pp->Description.indexOf(']', Pos);
			if(End < 0) return;  // should never happen
		}
		App->editText->setText(pp->Description.mid(Pos, End-Pos));
		App->editText->selectAll();
	}else{ untested();

	}
#endif
} // actionCursor

void SchematicDoc::actionApplyCompText()
{ untested();
#if 0 // what is focusElement??
	auto Doc=this;
	auto editText=App->editText;

	QString s;
	QFont f = QucsSettings.font;
	f.setPointSizeF(Scale * float(f.pointSize()) );
	editText->setFont(f);

	Property  *pp = 0;
	Component *pc = component(mouseActions()->focusElement);
	if(!pc) return;  // should never happen
	mouseActions()->MAx1 = pc->cx_() + pc->tx;
	mouseActions()->MAy1 = pc->cy_() + pc->ty;

	int z, n=0;  // "n" is number of property on screen
	pp = pc->Props.first();
	for(z=mouseActions()->MAx3; z>0; z--) {  // calculate "n"
		if(!pp) {  // should never happen
			App->hideEdit();
			return;
		}
		if(pp->display) n++;   // is visible ?
		pp = pc->Props.next();
	}

	pp = 0;
	if(mouseActions()->MAx3 > 0)  pp = pc->Props.at(mouseActions()->MAx3-1); // current property
	else s = pc->name();

	if(!editText->isHidden()) {   // is called the first time ?
		// no -> apply value to current property
		if(mouseActions()->MAx3 == 0) {   // component name ?
			if(!editText->text().isEmpty())
				if(pc->name() != editText->text()) { untested();
					auto pc2=Doc->find_component( editText->text());
					if(!pc2) { untested();
						pc->obsolete_name_override_hack( editText->text() );
						setChanged(true, true);  // only one undo state
					}
				}
		}
		else if(pp) {  // property was applied
			if(pp->Value != editText->text()) { untested();
				pp->Value = editText->text();
				recreateSymbol(pc);  // because of "Num" and schematic symbol
				setChanged(true, true); // only one undo state
			}
		}

		n++;     // next row on screen
		(mouseActions()->MAx3)++;  // next property
		pp = pc->Props.at(mouseActions()->MAx3-1);  // search for next property

		viewport()->update();
                mouseActions().setDrawn(false);

		if(!pp) {     // was already last property ?
			App->hideEdit();
			return;
		}


		while(!pp->display) {  // search for next visible property
			(mouseActions()->MAx3)++;  // next property
			pp = pc->Props.next();
			if(!pp) {     // was already last property ?
				App->hideEdit();
				return;
			}
		}
	}

	// avoid seeing the property text behind the line edit
	if(pp)  // Is it first property or component name ?
		s = pp->Value;
	editText->setMinimumWidth(editText->fontMetrics().width(s)+4);

  incomplete();
  /// Doc->contentsToViewport(int(Doc->Scale * float(view->MAx1 - Doc->ViewX1)),
  ///			 int(Doc->Scale * float(view->MAy1 - Doc->ViewY1)),
  ///	 view->MAx2, view->MAy2);
	editText->setReadOnly(false);
	if(pp) {  // is it a property ?
		s = pp->Value;
		mouseActions()->MAx2 += editText->fontMetrics().width(pp->Name+"=");
		if(pp->Description.indexOf('[') >= 0)  // is selection list ?
			editText->setReadOnly(true);
		Expr_CompProp.setPattern("[^\"]*");
		if(!pc->showName) n--;
	}
	else   // it is the component name
		Expr_CompProp.setPattern("[\\w_]+");
	Val_CompProp.setRegExp(Expr_CompProp);
	editText->setValidator(&Val_CompProp);

	z = editText->fontMetrics().lineSpacing();
	mouseActions()->MAy2 += n*z;
	editText->setText(s);
	misc::setWidgetBackgroundColor(editText, QucsSettings.BGColor);
	editText->setFocus();
	editText->selectAll();
	// make QLineEdit editable on mouse click
	QPoint p = QPoint(mouseActions()->MAx2, mouseActions()->MAy2);
	editText->setParent(Doc->viewport());
	App->editText->setGeometry(p.x(), p.y(), App->editText->width(), App->editText->height());
	App->editText->show();
#endif
}

// BUG: this is a diagram function.
void SchematicDoc::actionExportGraphAsCsv()
{ untested();
  // focusElement->exportGraphAsCsv? or so.
#if 0
  for(;;) { untested();
    if(!mouseActions()->focusElement){ untested();
    }else if(graph(mouseActions()->focusElement)){ untested();
        break;
    }else{ untested();
    }

    QMessageBox::critical(this, tr("Error"), tr("Please select a diagram graph!"));
    return;
  }

  /*QString s = Q3FileDialog::getSaveFileName(
     lastDir.isEmpty() ? QString(".") : lastDir,
     tr("CSV file")+" (*.csv);;" + tr("Any File")+" (*)",
     this, 0, tr("Enter an Output File Name"));
     */
  QString s = QFileDialog::getSaveFileName(this, tr("Enter an Output File Name"),
    lastDir.isEmpty() ? QString(".") : lastDir, tr("CSV file")+" (*.csv);;" + tr("Any File")+" (*)");

  if(s.isEmpty())
    return;

  QFileInfo Info(s);
  lastDir = Info.absolutePath();  // remember last directory
  if(Info.suffix().isEmpty())
    s += ".csv";

  QFile File(s);
  if(File.exists())
    if(QMessageBox::information(this, tr("Info"),
          tr("Output file already exists!")+"\n"+tr("Overwrite it?"),
          tr("&Yes"), tr("&No"), 0,1,1))
      return;

  if(!File.open(QIODevice::WriteOnly)) { untested();
    QMessageBox::critical(this, QObject::tr("Error"),
                          QObject::tr("Cannot create output file!"));
    return;
  }

  QTextStream Stream(&File);


  DataX const *pD;
  Graph const*g = graph(mouseActions()->focusElement);
  // First output the names of independent and dependent variables.
  for(unsigned ii=0; (pD=g->axis(ii)); ++ii){ untested();
    Stream << '\"' << pD->Var << "\";";
  }
  Stream << "\"r " << g->Var << "\";\"i " << g->Var << "\"\n";


  int n, m;
  double *py = g->cPointsY;
  int Count = g->countY * g->axis(0)->count;
  for(n = 0; n < Count; n++) { untested();
    m = n;
	 for(unsigned ii=0; (pD=g->axis(ii)); ++ii) { untested();
      Stream << *(pD->Points + m%pD->count) << ';';
      m /= pD->count;
    }

    Stream << *(py) << ';' << *(py+1) << '\n';
    py += 2;
  }

  File.close();
#endif
}

// -----------------------------------------------------------------------
/*!
 * \brief QucsApp::performToggleAction
 * \param on
 * \param Action
 * \param Function
 * \param MouseMove
 * \param MousePress
 * \return
 * This function is called from all toggle actions.
 * Used in combination with slots to set function pointers to the methods
 * that serve the mouse actions, ie. press, move, release, double click.
 */

#if 0
bool SchematicDoc::performToggleAction(bool on, QAction *Action,
	pToggleFunc Function, pMouseFunc MouseMove, pMouseFunc2 MousePress)
{ untested();
  assert(false); // obsolete
  assert(App);
  App->hideEdit(); // disable text edit of component property

  assert(on == Action.isChecked()); // why both?!

  if(!on) { untested();

    // toolbar button has been switched off.
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->MouseReleaseAction = 0;
    App->MouseDoubleClickAction = 0;
    App->activeAction = 0;

  }else{ untested();

    if(Function && (this->*Function)()) { untested();
      // some action "done"?? dont switch over to this one.
		// TODO
		//
      Action->blockSignals(true);
      Action->setChecked(false);  // release toolbar button
      Action->blockSignals(false);
      viewport()->update();
    }else{ untested();
      // action not "done"? leave active.

      if(App->activeAction) { untested();
        App->activeAction->blockSignals(true); // do not call toggle slot
        App->activeAction->setChecked(false);       // set last toolbar button off
        App->activeAction->blockSignals(false);
      }else{ untested();
      }

      App->activeAction = Action; /// this is a toolbar button
      App->MouseMoveAction = MouseMove;
      App->MousePressAction = MousePress;
      App->MouseReleaseAction = 0;
      App->MouseDoubleClickAction = 0;
    }

    viewport()->update();
    setDrawn(false);
    return true;
  }

  untested();
  return false; // really?
} // performtoggleAction
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
