import $ from 'jquery'

export class FlowGraphContextMenuEvent {
    constructor(positionX, positionY) {
        this.positionX = positionX;
        this.positionY = positionY;
    }
};

export class FlowGraphUpdatedNodeEvent {
    constructor(event, nodeId) {
        this.event = event;
        this.nodeId = nodeId;
    }
};

export class FlowGraphUpdatedConnectionEvent {
    constructor(event, connectionId) {
        this.event = event;
        this.connectionId = connectionId;
    }
};

export default class FlowGraph {
    constructor(rootNode, width, height, canvasOptions = { "width": 8096, "height": 8096 }) {
        this.rootNode = rootNode;
        this.width = width;
        this.height = height;
        this.canvasOptions = canvasOptions;

        this.nodes = [];
        this.nodesCache = {};

        this.connections = [];
        this.connectionsCache = {};

        this.focusedElement = null;
        this.selectedElement = null;
        this.dragMode = "none";
        this.zoomScale = 1.0;

        this.relativeCanvasX = canvasOptions.width / 2;
        this.relativeCanvasY = canvasOptions.height / 2;

        this.lastClientX = 0;
        this.lastClientY = 0;

        this.groups = {};

        this.lastCreatedId = 0;

        this.updatedConnectionCallback = null;
        this.updatedNodeCallback = null;
    }

    destructor() {

    }

    /**
     * Add new element to context menu
     * @param {string} group - Group of element
     * @param {string} item - Function name
     * @param {*} callback - Function callback
     */
    addContextItem(group, item, callback) {
        if (this.groups[group] == null) {
            this.groups[group] = [];
        }
        this.groups[group].push({ 'item': item, 'callback': callback });
    }

    resize(width, height) {
        $(this.rootNode)
            .css('width', width)
            .css('height', height);
        $('.flowgraph-bg-container')
            .css('width', width)
            .css('height', height);
        $('.flowgraph-info-container')
            .css('transform', `translate(${this.rootNode.offsetWidth - 200}px, ${this.rootNode.offsetHeight - 60}px)`);

        this.width = width;
        this.height = height;
    }

    start() {
        $(this.rootNode)
            .addClass('flowgraph-container')
            .css('width', this.width)
            .css('height', this.height);
        $(this.rootNode).append(`
            <div class="flowgraph-bg-container" 
                style="width: ${this.rootNode.style.width}; height: ${this.rootNode.style.height};"></div>
            <div class="flowgraph-canvas"
                style="width: ${this.canvasOptions.width}px; height: ${this.canvasOptions.height}px;">

                <svg class="flowgraph-vectorlayer" 
                    style="width: ${this.canvasOptions.width}px; height: ${this.canvasOptions.height}px;" xmlns="http://www.w3.org/2000/svg"></svg>
            </div>
        `);
        $(this.rootNode).append(`
            <div class="flowgraph-context-container" style="display: none;">
                <div class="flowgraph-context-search">
                    <object data="images/search.svg" width="16" height="16"> </object>
                    <input type="text" placeholder="Type to search" search />
                </div>
                <div class="flowgraph-context-main"></div>
            </div>
        `);
        $('.flowgraph-context-search input').bind('input', e => { this.#renderContextMenu(); });
        $(this.rootNode).append(`
            <div class="flowgraph-info-container"
                style="transform: translate(${this.rootNode.offsetWidth - 200}px, ${this.rootNode.offsetHeight - 60}px);">
                
                <div class="flowgraph-info-main">
                    <span>X: ${0} Y: ${0}</span>
                    <span>Zoom: ${this.zoomScale.toFixed(2)}</span>
                </div>
            </div>
        `)

        this.rootNode.addEventListener('mousewheel', this.#zoomEventHandler.bind(this));
        this.rootNode.addEventListener('mousedown', event => {
            if (event.which == 1) {
                this.#dragNodeBeginHandler(event);
            } else if (event.which == 3) {
                this.#rightClickHandler(event);
            }
        });
        this.rootNode.addEventListener('mouseup', this.#dragNodeEndHandler.bind(this));
        this.rootNode.addEventListener('mousemove', this.#moveNodeHandler.bind(this));
        //this.rootNode.addEventListener('contextmenu', event => event.preventDefault());

        document.addEventListener('keydown', this.#keyHandler.bind(this));

        $(this.rootNode).children('.flowgraph-canvas').css('transform', `translate(${-this.relativeCanvasX}px, ${-this.relativeCanvasY}px)`);
    }

    #renderContextMenu() {
        let contextMenuElement = $('.flowgraph-context-main')
            .empty();

        const canvasMatrix = $('.flowgraph-canvas')
            .css('transform')
            .match(/-?[\d\.]+/g);

        const posX = this.lastClientX + (-this.relativeCanvasX - Number(canvasMatrix[4]));
        const posY = this.lastClientY + (-this.relativeCanvasY - Number(canvasMatrix[5]));

        const searchPattern = $('.flowgraph-context-search input').get(0).value;
        let filteredGroups;
        if (searchPattern.length == 0) {
            filteredGroups = Object.entries(this.groups);
        } else {
            filteredGroups = Object.entries(this.groups).filter(
                (key, value) => (
                    Object.values(this.groups["Math"]).filter(
                        value => String(value.item).includes(searchPattern)
                    ).length > 0 | String(key).includes(searchPattern)
                )
            );
        }

        for (const [group, data] of filteredGroups) {
            contextMenuElement
                .append(`<label>${group}</label>`)
                .append(`<div class="flowgraph-context-group"></div>`);

            let groupMenuElement = $('.flowgraph-context-group').last();
            for (const item of data) {
                groupMenuElement.append(`<button>${item.item}</button>`);
                $('.flowgraph-context-group button')
                    .last()
                    .bind('click', e => {
                        item.callback(new FlowGraphContextMenuEvent(posX, posY));
                        $(this.rootNode).children('.flowgraph-context-container')
                            .css('display', 'none');
                    });
            }
            contextMenuElement.append(`</div>`);
        }
    }

    #keyHandler(e) {
        if (e.code == 'Backspace' || e.code == 'Delete') {
            if (this.focusedElement) {
                if ($(this.focusedElement).closest('.flowgraph-node-container').length > 0) {
                    const nodeId = this.focusedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    this.removeNode(nodeId);
                } else if ($(this.focusedElement).closest('.flowgraph-connection').length > 0) {
                    const connectionId = this.focusedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    this.removeConnection(connectionId);
                }
            }
        }
    }

    #rightClickHandler(e) {
        if ($(e.target).closest('.flowgraph-context-container').length > 0) {

        } else {
            $(this.rootNode).children('.flowgraph-context-container')
                .css('display', 'block')
                .css('transform', `translate(${this.lastClientX}px, ${this.lastClientY}px)`);

            this.#renderContextMenu();
        }
    }

    #dragNodeBeginHandler(e) {
        if ($(e.target).closest('.flowgraph-io').closest('.flowgraph-io-row.left').length > 0) {
            this.dragMode = "io.input";
        } else if ($(e.target).closest('.flowgraph-io').closest('.flowgraph-io-row.right').length > 0) {
            this.dragMode = "io.output";
        } else if ($(e.target).closest('.flowgraph-node-container').length > 0) {
            this.dragMode = "node";
        } else if ($(e.target).closest('.flowgraph-connection').length > 0) {
            this.dragMode = "connection";
        } else if ($(e.target).closest('.flowgraph-canvas').length > 0) {
            this.dragMode = "canvas";
        }

        if ($(e.target).closest('.flowgraph-context-container').length > 0) {

        } else {
            $(this.rootNode).children('.flowgraph-context-container')
                .css('display', 'none');
        }

        if (this.focusedElement) {
            if ($(this.focusedElement).closest('.flowgraph-connection').length > 0) {
                this.focusedElement.setAttribute('stroke', 'rgb(42, 143, 42)');
                this.focusedElement.setAttribute('stroke-width', '8');
            } else {
                $(this.focusedElement).removeClass('focused');
            }
            this.focusedElement = null;
        }

        switch (this.dragMode) {
            case "io.input": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-io')
                    .get(0);

                const [destNode, inIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex))

                if (connection) {
                    $(`#node_${destNode}_in_${inIndex}`)
                        .removeClass('connected')
                        .get(0);
                }

                const elementMatrix = $(this.selectedElement)
                    .closest('.flowgraph-node-container')
                    .css('transform')
                    .match(/-?[\d\.]+/g);

                const posX = Number(elementMatrix[4]) + this.selectedElement.offsetLeft;
                const posY = Number(elementMatrix[5]) + this.selectedElement.offsetTop;

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .append(`
                        <div id="drag-io-dummy" class="flowgraph-io circle" style="pointer-events: none; position: absolute; opacity: 0; transform: translate(${posX}px, ${posY}px)"></div>
                    `);
                break;
            }
            case "io.output": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-io')
                    .get(0);

                const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                this.#addPreviewConnection(sourceNode, outIndex);

                const elementMatrix = $(this.selectedElement)
                    .closest('.flowgraph-node-container')
                    .css('transform')
                    .match(/-?[\d\.]+/g);

                const posX = Number(elementMatrix[4]) + this.selectedElement.offsetLeft;
                const posY = Number(elementMatrix[5]) + this.selectedElement.offsetTop;

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .append(`
                        <div id="drag-io-dummy" class="flowgraph-io circle" style="pointer-events: none; position: absolute; opacity: 0; transform: translate(${posX}px, ${posY}px)"></div>
                    `)
                break;
            }
            case "node": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-node-container')
                    .get(0);

                $(this.selectedElement).addClass('selected');
                break;
            }
            case "canvas": {
                break;
            }
            case "connection": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-connection')
                    .get(0);
                break;
            }
        }
    }

    #dragNodeEndHandler(e) {
        switch (this.dragMode) {
            case "io.input": {
                if ($(e.target).closest('.flowgraph-io').closest('.flowgraph-io-row.left').length > 0) {
                    let destElement = $(e.target).closest('.flowgraph-io')[0];

                    const [destNode, inIndex] = destElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    let connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        this.removeConnection(connection.id);
                    }

                    const [selectedNode, selectedIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    connection = Object.values(this.connections).find(value => (value.dest == selectedNode && value.in == selectedIndex));

                    if (connection) {
                        this.#updateConnection(connection.id, connection.source, connection.out, destNode, inIndex);
                    }
                } else {
                    const [destNode, inIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        this.removeConnection(connection.id);
                    }
                }

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .children('#drag-io-dummy')
                    .remove();
                break;
            }
            case "io.output": {
                if ($(e.target).closest('.flowgraph-io').closest('.flowgraph-io-row.left').length > 0) {
                    let destElement = $(e.target).closest('.flowgraph-io')[0];

                    const [destNode, inIndex] = destElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        if (this.#nodeInputTypeCheck(sourceNode, outIndex, destNode, inIndex)) {
                            const connections = Object.values(this.connections).filter(
                                value => (value.source == this.connections[this.connectionsCache[connection.id]].source && value.out == this.connections[this.connectionsCache[connection.id]].out));
                            if (connections.length == 1) {
                                $(`#node_${this.connections[this.connectionsCache[connection.id]].source}_out_${this.connections[this.connectionsCache[connection.id]].out}`)
                                    .removeClass('connected')
                                    .get(0);
                            }

                            this.#updateConnection(connection.id, sourceNode, outIndex, destNode, inIndex);
                        }
                    } else {
                        try {
                            this.addConnection(sourceNode, outIndex, destNode, inIndex);
                        } catch (e) {
                            const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));

                            const connections = Object.values(this.connections).filter(
                                value => (value.source == sourceNode && value.out == outIndex));
                            if (connections.length == 0) {
                                $(`#node_${sourceNode}_out_${outIndex}`)
                                    .removeClass('connected')
                                    .get(0);
                            }
                        }
                    }
                } else {
                    const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));

                    const connections = Object.values(this.connections).filter(
                        value => (value.source == sourceNode && value.out == outIndex));
                    if (connections.length == 0) {
                        $(`#node_${sourceNode}_out_${outIndex}`)
                            .removeClass('connected')
                            .get(0);
                    }
                }

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .children('#drag-io-dummy')
                    .remove();

                const vectorLayer = $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .children('.flowgraph-vectorlayer')
                    .get(0);

                let svgElement = vectorLayer.getElementById(`connection_preview`);
                svgElement.remove();
                break;
            }
            case "node": {
                if (this.selectedElement) {
                    this.focusedElement = this.selectedElement;
                    $(this.focusedElement).addClass('focused');

                    $(this.selectedElement).removeClass('selected');
                }
                break;
            }
            case "connection": {
                if (this.selectedElement) {
                    this.focusedElement = this.selectedElement;
                    this.focusedElement.setAttribute('stroke', 'rgb(180, 180, 180)');
                    this.focusedElement.setAttribute('stroke-width', '10');
                }
                break;
            }
        }

        this.dragMode = "none";
        this.selectedElement = null;
    }

    #nodeInputTypeCheck(sourceNode, outIndex, destNode, inIndex) {
        return this.nodes[this.nodesCache[sourceNode]].outputs[outIndex].type == this.nodes[this.nodesCache[destNode]].inputs[inIndex].type;
    }

    #moveNodeHandler(e) {
        const relativeClientX = e.clientX - this.lastClientX;
        const relativeClientY = e.clientY - this.lastClientY;
        this.lastClientX = e.clientX;
        this.lastClientY = e.clientY;

        switch (this.dragMode) {
            case "canvas": {
                const posMatrix = $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .css('transform')
                    .match(/-?[\d\.]+/g);

                const posX = Number(posMatrix[4]) + relativeClientX;
                const posY = Number(posMatrix[5]) + relativeClientY;

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .css('transform', `translate(${posX}px, ${posY}px) scale(${this.zoomScale})`);

                $('.flowgraph-info-main')
                    .empty()
                    .append(`
                        <span>X: ${-this.relativeCanvasX - posX} Y: ${-this.relativeCanvasX - posY}</span>
                        <span>Zoom: ${this.zoomScale.toFixed(2)}</span>
                    `);
                break;
            }
            case "node": {
                if (this.selectedElement) {
                    const posMatrix = $(this.selectedElement)
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const posX = Number(posMatrix[4]) + relativeClientX / this.zoomScale;
                    const posY = Number(posMatrix[5]) + relativeClientY / this.zoomScale;

                    $(this.selectedElement).css('transform', `translate(${posX}px, ${posY}px)`);

                    const nodeId = Number(this.selectedElement.id.match(/-?[\d]/g)[0]);
                    const connections = Object.values(this.connections).filter(value => (value.source == nodeId || value.dest == nodeId));

                    for (const [_, value] of Object.entries(connections)) {
                        const sourceElement = $(`#node_${this.connections[this.connectionsCache[value.id]].source}_out_${this.connections[this.connectionsCache[value.id]].out}`).get(0);
                        const sourceMatrix = $(`#node_${this.connections[this.connectionsCache[value.id]].source}`)
                            .css('transform')
                            .match(/-?[\d\.]+/g);

                        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
                        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

                        const destElement = $(`#node_${this.connections[this.connectionsCache[value.id]].dest}_in_${this.connections[this.connectionsCache[value.id]].in}`).get(0);
                        const destMatrix = $(`#node_${this.connections[this.connectionsCache[value.id]].dest}`)
                            .css('transform')
                            .match(/-?[\d\.]+/g);

                        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
                        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

                        this.#updateConnectionPosition(value.id, sourceX, sourceY, destX, destY);
                    }
                }
                break;
            }
            case "io.input": {
                if (this.selectedElement) {
                    const posMatrix = $(this.rootNode)
                        .children('.flowgraph-canvas')
                        .children('#drag-io-dummy')
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const posX = Number(posMatrix[4]) + relativeClientX / this.zoomScale;
                    const posY = Number(posMatrix[5]) + relativeClientY / this.zoomScale;

                    $(this.rootNode)
                        .children('.flowgraph-canvas')
                        .children('#drag-io-dummy')
                        .css('transform', `translate(${posX}px, ${posY}px)`);

                    const [destNode, inIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        const sourceElement = $(`#node_${this.connections[this.connectionsCache[connection.id]].source}_out_${this.connections[this.connectionsCache[connection.id]].out}`).get(0);
                        const sourceMatrix = $(`#node_${this.connections[this.connectionsCache[connection.id]].source}`)
                            .css('transform')
                            .match(/-?[\d\.]+/g);

                        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
                        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;
                        const destX = posX;
                        const destY = posY;

                        this.#updateConnectionPosition(connection.id, sourceX, sourceY, destX, destY);
                    }
                }
                break;
            }
            case "io.output": {
                if (this.selectedElement) {
                    const posMatrix = $(this.rootNode)
                        .children('.flowgraph-canvas')
                        .children('#drag-io-dummy')
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const posX = Number(posMatrix[4]) + relativeClientX / this.zoomScale;
                    const posY = Number(posMatrix[5]) + relativeClientY / this.zoomScale;

                    $(this.rootNode)
                        .children('.flowgraph-canvas')
                        .children('#drag-io-dummy')
                        .css('transform', `translate(${posX}px, ${posY}px)`);

                    const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));

                    const sourceElement = $(`#node_${sourceNode}_out_${outIndex}`).get(0);
                    const sourceMatrix = $(`#node_${sourceNode}`)
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
                    const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;
                    const destX = posX;
                    const destY = posY;

                    this.#updateConnectionPreviewPosition(sourceX, sourceY, destX, destY);
                }
                break;
            }
        }
    }

    #zoomEventHandler(e) {
        if ($(e.target).closest('.flowgraph-context-container').length > 0) {

        } else {
            e.preventDefault();

            if (e.deltaY > 0) {
                if (this.zoomScale > 0.4) {
                    this.zoomScale -= 0.05;
                }
            } else {
                if (this.zoomScale < 1.3) {
                    this.zoomScale += 0.05;
                }
            }

            const posMatrix = $(this.rootNode)
                .children('.flowgraph-canvas')
                .css('transform')
                .match(/-?[\d\.]+/g);

            const posX = Number(posMatrix[4]);
            const posY = Number(posMatrix[5]);

            $(this.rootNode)
                .children('.flowgraph-canvas')
                .css('transform', `translate(${posX}px, ${posY}px) scale(${this.zoomScale})`);

            $('.flowgraph-info-main')
                .empty()
                .append(`
                    <span>X: ${-this.relativeCanvasX - posX} Y: ${-this.relativeCanvasX - posY}</span>
                    <span>Zoom: ${this.zoomScale.toFixed(2)}</span>
                `);
        }
    }

    removeConnection(connectionId) {
        if (this.connectionsCache[connectionId] == null) {
            throw new Error("Connection with this ID does not exist");
        }

        const connections = Object.values(this.connections).filter(
            value => (value.source == this.connections[this.connectionsCache[connectionId]].source && value.out == this.connections[this.connectionsCache[connectionId]].out));
        if (connections.length == 1) {
            $(`#node_${this.connections[this.connectionsCache[connectionId]].source}_out_${this.connections[this.connectionsCache[connectionId]].out}`)
                .removeClass('connected')
                .get(0);
        }

        $(`#node_${this.connections[this.connectionsCache[connectionId]].dest}_in_${this.connections[this.connectionsCache[connectionId]].in}`)
            .removeClass('connected')
            .get(0);

        const vectorLayer = $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .get(0);

        let svgElement = vectorLayer.getElementById(`connection_${connectionId}`);
        svgElement.remove();

        this.connections.splice(this.connectionsCache[connectionId], 1);

        this.connectionsCache = {};
        for (let i = 0; i < this.connections.length; i++) {
            this.connectionsCache[this.connections[i].id] = i;
        }
    }

    #updateConnectionPosition(connectionId, sourceX, sourceY, destX, destY) {
        const controlX = (sourceX + destX) / 2;
        const controlY = (sourceY + destY) / 2;

        const vectorLayer = $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .get(0);

        let svgElement = vectorLayer.getElementById(`connection_${connectionId}`);
        svgElement.setAttribute('d', `M ${sourceX} ${sourceY} Q ${controlX} ${controlY} ${destX} ${destY}`);
    }

    #addPreviewConnection(sourceNode, outIndex) {
        $(`#node_${sourceNode}_out_${outIndex}`)
            .addClass('connected')
            .get(0);

        let svgElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
        svgElement.classList.add('flowgraph-connection');
        svgElement.setAttribute('id', `connection_preview`);
        svgElement.setAttribute('stroke', 'rgb(42, 143, 42)');
        svgElement.setAttribute('stroke-width', '8');
        svgElement.setAttribute('fill', 'transparent');

        $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .append(svgElement);
    }

    #updateConnectionPreviewPosition(sourceX, sourceY, destX, destY) {
        const controlX = (sourceX + destX) / 2;
        const controlY = (sourceY + destY) / 2;

        const vectorLayer = $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .get(0);
        let svgElement = vectorLayer.getElementById(`connection_preview`);
        svgElement.setAttribute('d', `M ${sourceX} ${sourceY} Q ${controlX} ${controlY} ${destX} ${destY}`);
    }

    #updateConnection(connectionId, sourceNodeId, outIndex, destNodeId, inIndex) {
        const sourceElement = $(`#node_${sourceNodeId}_out_${outIndex}`)
            .addClass('connected')
            .get(0);
        const sourceMatrix = $(`#node_${sourceNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

        const destElement = $(`#node_${destNodeId}_in_${inIndex}`)
            .addClass('connected')
            .get(0);
        const destMatrix = $(`#node_${destNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

        this.connections[this.connectionsCache[connectionId]] = {
            "id": connectionId,
            "source": sourceNodeId,
            "out": outIndex,
            "dest": destNodeId,
            "in": inIndex
        };

        this.#updateConnectionPosition(connectionId, sourceX, sourceY, destX, destY);
    }

    #internalAddConnection(id, sourceNodeId, outIndex, destNodeId, inIndex) {
        if (!this.#nodeInputTypeCheck(sourceNodeId, outIndex, destNodeId, inIndex)) {
            throw new TypeError("Cannot join different data types");
        }

        const sourceElement = $(`#node_${sourceNodeId}_out_${outIndex}`)
            .addClass('connected')
            .get(0);
        const sourceMatrix = $(`#node_${sourceNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

        const destElement = $(`#node_${destNodeId}_in_${inIndex}`)
            .addClass('connected')
            .get(0);
        const destMatrix = $(`#node_${destNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

        let svgElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
        svgElement.classList.add('flowgraph-connection');
        svgElement.setAttribute('id', `connection_${id}`);
        svgElement.setAttribute('stroke', 'rgb(42, 143, 42)');
        svgElement.setAttribute('stroke-width', '8');
        svgElement.setAttribute('fill', 'transparent');

        $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .append(svgElement);

        const size = this.connections.push({
            "id": id,
            "source": sourceNodeId,
            "out": outIndex,
            "dest": destNodeId,
            "in": inIndex
        });

        this.connectionsCache[id] = size - 1;

        this.#updateConnectionPosition(id, sourceX, sourceY, destX, destY);
    }

    addConnection(sourceNodeId, outIndex, destNodeId, inIndex) {
        const id = this.lastCreatedId;
        this.#internalAddConnection(id, sourceNodeId, outIndex, destNodeId, inIndex);
        this.lastCreatedId++;
    }

    removeNode(nodeId) {
        if (this.nodesCache[nodeId] == null) {
            throw new Error("Node with this ID does not exist");
        }

        const connections = Object.values(this.connections).filter(value => (value.source == nodeId || value.dest == nodeId));
        for (const value of Object.values(connections)) {
            this.removeConnection(value.id);
        }

        $(`#node_${nodeId}`)
            .get(0)
            .remove();

        this.nodes.splice(this.nodesCache[nodeId], 1)

        this.nodesCache = {};
        for (let i = 0; i < this.nodes.length; i++) {
            this.nodesCache[this.nodes[i].id] = i;
        }
    }

    #internalAddNode(id, x, y, inputs, outputs, nodeName, isExpand, expandHTML) {
        let inputsHTML = "";
        for (let i = 0; i < inputs.length; i++) {
            inputsHTML += `
                <div class="flowgraph-io-row left">
                    <div id="node_${id}_in_${i}" class="flowgraph-io circle"></div>
                    <span style="color: white;">${inputs[i].name} (${inputs[i].type})</span>
                </div>
            `;
        }

        let outputsHTML = "";
        for (let i = 0; i < outputs.length; i++) {
            outputsHTML += `
                <div class="flowgraph-io-row right">
                    <span style="color: white;">${outputs[i].name} (${outputs[i].type})</span>
                    <div id="node_${id}_out_${i}" class="flowgraph-io circle"></div>
                </div>
            `;
        }

        let bodyHTML = "";
        if (inputsHTML.length > 0) {
            bodyHTML += `
                <div class="flowgraph-io-list">
                    ${inputsHTML}
                </div>
            `;
        }
        if (outputsHTML.length > 0) {
            bodyHTML += `
                <div class="flowgraph-io-list">
                    ${outputsHTML}
                </div>
            `;
        }

        let footerHTML = "";
        if (isExpand) {
            footerHTML += `
                <div class="flowgraph-node-footer">
                    <div class="flowgraph-expand-container">
                        <object data="images/angle-down.svg" width="16" height="16" style="pointer-events: none;"></object>
                    </div>
                </div>
                <div class="flowgraph-hidden-container" style="display: none;"></div>
            `;
        }

        $(this.rootNode).children('.flowgraph-canvas').append(`
            <div id="node_${id}" class="flowgraph-node-container"> 
                <div class="flowgraph-node-root">
                    <div class="flowgraph-node-header">
                        <span style="color: white;">${nodeName}</span>
                    </div>

                    <div class="flowgraph-node-main">
                        ${bodyHTML}
                    </div>

                    ${footerHTML}
                </div>
            </div>
        `);

        let nodeWidth = 110 + Math.min(1, outputs.length) * 120 + Math.min(1, inputs.length) * 120;

        let nodeHeight = $(`#node_${id}`).find('.flowgraph-node-header').outerHeight() +
            $(`#node_${id}`).find('.flowgraph-node-main').outerHeight();
        if (isExpand) {
            nodeHeight += $(`#node_${id}`).find('.flowgraph-node-footer').outerHeight();
        }

        $(`#node_${id}`)
            .css('width', `${nodeWidth}px`)
            .css('height', `${nodeHeight}px`);

        $(`#node_${id}`).find('.flowgraph-expand-container').bind(
            'click',
            e => {
                let hiddenContainer = $(`#node_${id}`).find('.flowgraph-hidden-container')
                if (hiddenContainer.css('display') == 'flex') {
                    let nodeHeight = $(`#node_${id}`).find('.flowgraph-node-header').outerHeight() +
                        $(`#node_${id}`).find('.flowgraph-node-main').outerHeight() +
                        $(`#node_${id}`).find('.flowgraph-node-footer').outerHeight();

                    $(`#node_${id}`).css('height', `${nodeHeight}px`);
                    hiddenContainer.css('display', 'none');
                    $(`#node_${id}`)
                        .find('.flowgraph-expand-container')
                        .children('object')
                        .replaceWith('<object data="images/angle-down.svg" width="16" height="16" style="pointer-events: none;"></object>');
                } else {
                    let nodeHeight = $(`#node_${id}`).find('.flowgraph-node-header').outerHeight() +
                        $(`#node_${id}`).find('.flowgraph-node-main').outerHeight() +
                        $(`#node_${id}`).find('.flowgraph-node-footer').outerHeight() + 150;

                    $(`#node_${id}`).css('height', `${nodeHeight}px`);
                    hiddenContainer.css('display', 'flex');
                    $(`#node_${id}`)
                        .find('.flowgraph-expand-container')
                        .children('object')
                        .replaceWith('<object data="images/angle-up.svg" width="16" height="16" style="pointer-events: none;"></object>');
                }
            });

        $(`#node_${id}`).css('transform', `translate(${this.relativeCanvasX + x}px, ${this.relativeCanvasY + y}px)`);

        const size = this.nodes.push({
            "id": id,
            "position": [x, y],
            "inputs": inputs,
            "outputs": outputs
        });

        this.nodesCache[id] = size - 1;
    }

    /**
     * Bind callbacks
     * @param {int} eventName - UpdatedNode or UpdatedConnection
     * @param {int} func - Binded callback
     * @returns 
     */
    bind(eventName, func) {
        switch(eventName) {
            case "UpdatedNode": {
                this.updatedNodeCallback = func;
                break;
            }
            case "UpdatedConnection": {
                this.updatedConnectionCallback = func;
                break;
            }
            default: {
                throw new Error("eventName is unknown");
            }
        }
    }

    /**
     * Set title to node
     * @param {int} nodeId - ID node
     * @param {int} title - Title
     * @returns 
     */
    setTitleNode(nodeId, title) {
        if (this.nodesCache[nodeId] == null) {
            throw new Error("Node with this ID does not exist");
        }

        $(`#node_${nodeId}`)
            .find(".flowgraph-node-header")
            .children('span')
            .html(title);
    }

    /**
     * Add node to graph
     * @param {int} x - X node position
     * @param {int} y - Y node position
     * @param {map} inputs - Input parameters
     * @param {map} outputs - Output parameters
     * @param {string} nodeName - Title in plaintext
     * @param {bool} isExpand - Expand node for additional information
     * @param {string} expandHTML - Expand body in HTML
     * @returns 
     */
    addNode(x, y, inputs, outputs, nodeName, isExpand, expandHTML) {
        const id = this.lastCreatedId;
        this.#internalAddNode(id, x, y, inputs, outputs, nodeName, isExpand, expandHTML);
        this.lastCreatedId++;
    }

    importFromJSON(data) {
        this.nodes = [];
        this.nodesCache = {};
        this.connections = [];
        this.connectionsCache = {};

        for (const sceneNode of Object.values(data["sceneNodes"])) {
            let inputsData = [];

            for (let i = 0; i < sceneNode.inputs.length; i++) {
                inputsData.push({
                    "name": sceneNode.inputs[i].socketName,
                    "type": sceneNode.inputs[i].socketType
                });
            }

            let outputsData = [];

            for (let i = 0; i < sceneNode.outputs.length; i++) {
                outputsData.push({
                    "name": sceneNode.outputs[i].socketName,
                    "type": sceneNode.outputs[i].socketType
                });
            }

            this.#internalAddNode(
                sceneNode.nodeID,
                sceneNode.position[0],
                sceneNode.position[1],
                inputsData,
                outputsData,
                sceneNode.nodeName,
                sceneNode.isExpand,
                "");
        }

        for (const sceneConnection of Object.values(data["sceneConnections"])) {

            this.#internalAddConnection(
                sceneConnection.connectionID,
                sceneConnection.sourceNode,
                sceneConnection.sourceIndex,
                sceneConnection.destNode,
                sceneConnection.destIndex);
        }

        const connectionsIds = Object.keys(this.connectionsCache).map(key => parseInt(key));
        const nodesIds = Object.keys(this.nodesCache).map(key => parseInt(key));
        this.lastCreatedId = Math.max(...connectionsIds, ...nodesIds) + 1;
    }
}