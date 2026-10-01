function showBranchAndBoundVisualization(searchTree) {
    const container = document.getElementById("visualization");

    if (!container) {
        return;
    }

    container.innerHTML = "";

    if (!Array.isArray(searchTree) || searchTree.length === 0) {
        container.innerHTML = "<p>No visualization data available.</p>";
        return;
    }

    const title = document.createElement("h3");
    title.textContent = "Branch and Bound Search";
    container.appendChild(title);

    const tree = document.createElement("div");
    tree.className = "search-tree";

    searchTree.forEach((node, index) => {
        const nodeBox = document.createElement("div");
        nodeBox.className = "search-node";

        const heading = document.createElement("h4");
        heading.textContent = `Node ${index + 1}`;

        const level = document.createElement("p");
        level.textContent = `Level: ${node.level}`;

        const currentTime = document.createElement("p");
        currentTime.textContent =
            `Current Time: ${node.currentTime}`;

        const currentValue = document.createElement("p");
        currentValue.textContent =
            `Current Value: ${node.currentValue}`;

        const bound = document.createElement("p");
        bound.textContent =
            `Bound: ${Number(node.bound).toFixed(2)}`;

        const places = document.createElement("p");

        if (
            Array.isArray(node.selectedPlaces) &&
            node.selectedPlaces.length > 0
        ) {
            places.textContent =
                `Selected: ${node.selectedPlaces.join(", ")}`;
        } else {
            places.textContent = "Selected: None";
        }

        nodeBox.appendChild(heading);
        nodeBox.appendChild(level);
        nodeBox.appendChild(currentTime);
        nodeBox.appendChild(currentValue);
        nodeBox.appendChild(bound);
        nodeBox.appendChild(places);

        tree.appendChild(nodeBox);
    });

    container.appendChild(tree);
}

function showRoute(route) {
    const container = document.getElementById("routeVisualization");

    if (!container) {
        return;
    }

    container.innerHTML = "";

    if (!Array.isArray(route) || route.length === 0) {
        container.innerHTML = "<p>No route available.</p>";
        return;
    }

    const routeContainer = document.createElement("div");
    routeContainer.className = "route-container";

    route.forEach((location, index) => {
        const locationBox = document.createElement("div");
        locationBox.className = "route-location";

        locationBox.textContent = location;

        routeContainer.appendChild(locationBox);

        if (index < route.length - 1) {
            const arrow = document.createElement("span");
            arrow.className = "route-arrow";
            arrow.textContent = "→";

            routeContainer.appendChild(arrow);
        }
    });

    container.appendChild(routeContainer);
}

window.showBranchAndBoundVisualization =
    showBranchAndBoundVisualization;

window.showRoute = showRoute;
