# JSON Tree Viewer

#### **JSON Tree Viewer** is a lightweight and powerful Unreal Engine plugin that renders structured JSON data as an interactive, collapsible tree—perfect for in-editor tools or runtime data visualization. Feed it a raw JSON string or a file path, and the plugin will do the rest.

![image](https://github.com/rohan-singh1/JsonTreeViewerDemo/blob/f57a643daeb7c99ce8ffdf2f1c144b9b55e0b52a/Resources/Thumbnail_128.png)


##  Features

- **Seamless Integration**  
  Easily used as a `UWidget` in UMG or as a Blueprint-exposed component in your tools.

- **Customizable Appearance**  
  Style every part of your JSON: key/value colors, fonts, padding, and special highlighting for `@`-prefixed keys.

- **Full JSON Type Support**  
  Handles all types: objects, arrays, strings, numbers, booleans, and nulls.

- **Blueprint Friendly**  
  Load JSON files and initialize trees directly from Blueprint graphs with simple function calls.

- **Ideal for:**
  - Inspecting configuration files and payloads
  - Visualizing live or saved JSON at runtime or in editor
  - Building data-driven debugging UIs
  - Enhancing editor utility widgets

---

##  Getting Started

### 1. Install the Plugin

Place the plugin inside your project’s `Plugins` directory:

```
MyProject/
└── Plugins/
    └── JsonTreeViewer/
```

Enable it in **Edit → Plugins** and restart the editor.

---

### 2. Usage

#### a. In UMG

- Add a `JsonTreeViewerWidget` to your widget hierarchy.
- Set `JsonInput` to a valid `.json` file path or raw JSON string.
- Optionally, configure styling using exposed color/font/padding properties.

#### b. In Blueprints

Use the following Blueprint-callable function:

- `InitJsonTree(JsonStringOrPath)` – Initializes the tree with a string or file path

---

##  Properties

| Property              | Description                                      |
|-----------------------|--------------------------------------------------|
| `JsonInput`           | Raw JSON string or valid file path              |
| `KeyColor`            | Color for regular keys                          |
| `KeyAtColor`          | Color for keys starting with `@`                |
| `StringValueColor`    | Color for string values                         |
| `BooleanValueColor`   | Color for boolean values                        |
| `NumericValueColor`   | Color for numeric values                        |
| `NullValueColor`      | Color for `null`                                |
| `Padding`             | Padding between tree row widgets                |
| `Font`                | Font used for displaying keys and values        |

---

##  Example Blueprint Setup

1. Drag the widget into your UMG layout.
2. On `Event Construct`:
   - Use `InitJsonTree(JsonString)` to load a file or JSON string to visualize.

---

##  Under the Hood

- Powered by `STreeView` (Slate).
- Internally parses `FJsonValue` into a recursive `FTreeItem` hierarchy.
- Assigns unique Slate color styles based on JSON value types.
- Automatically expands nested JSON objects and arrays into children.

---

##  Author

**Rohan Singh**  
 [GitHub](https://github.com/rohan-singh1)  
 [UI Portfolio](https://new.express.adobe.com/webpage/QHNRXl0ZDOBxJ)  
 [Fab Marketplace](https://www.fab.com/sellers/Rohan%20Singh)

---

##  Screenshots
![image](https://github.com/rohan-singh1/JsonTreeViewerDemo/blob/f57a643daeb7c99ce8ffdf2f1c144b9b55e0b52a/Resources/1.png)
---
![image](https://github.com/rohan-singh1/JsonTreeViewerDemo/blob/f57a643daeb7c99ce8ffdf2f1c144b9b55e0b52a/Resources/2.png)
---
![image](https://github.com/rohan-singh1/JsonTreeViewerDemo/blob/f57a643daeb7c99ce8ffdf2f1c144b9b55e0b52a/Resources/3.png)
---
