import os
import json
import requests
from github import Github

# 1. Cargar variables de entorno
OPENROUTER_API_KEY = os.getenv("OPENROUTER_API_KEY")
GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")
REPO_NAME = os.getenv("GITHUB_REPOSITORY")
ISSUE_NUMBER = int(os.getenv("ISSUE_NUMBER"))
ISSUE_TITLE = os.getenv("ISSUE_TITLE", "")
ISSUE_BODY = os.getenv("ISSUE_BODY", "")

# Lista de colaboradores disponibles para asignar
TEAM_MEMBERS = ["amf162-ua", "omr10-ua", "ikerroig", "evfv2-ua", "jpt35-ua"]
AVAILABLE_LABELS = ["bug", "documentation", "enhancement", "critical", "frontend", "backend"]

def analyze_issue_with_openrouter(title, body):
    prompt = f"""
    Eres un asistente de triaje de proyectos de software.
    Analiza el siguiente issue y responde ÚNICAMENTE con un JSON válido (sin markdown ni bloques de código ```json):

    Título: {title}
    Descripción: {body}

    Opciones de etiquetas disponibles: {json.dumps(AVAILABLE_LABELS)}
    Opciones de miembros del equipo: {json.dumps(TEAM_MEMBERS)}

    Formato de respuesta requerido:
    {{
      "labels": ["etiqueta1", "etiqueta2"],
      "assignee": "nombre_usuario_o_null",
      "comment": "Explicación breve o análisis diagnóstico para el issue."
    }}
    """

    headers = {
        "Authorization": f"Bearer {OPENROUTER_API_KEY}",
        "Content-Type": "application/json"
    }

    data = {
        "model": "meta-llama/llama-3.3-70b-instruct",
        "messages": [{"role": "user", "content": prompt}],
        "temperature": 0.2
    }

    response = requests.post("[https://openrouter.ai/api/v1/chat/completions](https://openrouter.ai/api/v1/chat/completions)", headers=headers, json=data)
    response.raise_for_status()
    
    content = response.json()["choices"][0]["message"]["content"].strip()
    # Limpiar formato si el modelo devuelve delimitadores markdown
    if content.startswith("```"):
        content = content.split("\n", 1)[1].rsplit("\n", 1)[0]
    
    return json.loads(content)

def main():
    print(f"Iniciando triaje para Issue #{ISSUE_NUMBER} en {REPO_NAME}...")
    
    # Obtener análisis de IA
    ai_result = analyze_issue_with_openrouter(ISSUE_TITLE, ISSUE_BODY)
    print(f"Resultado IA: {ai_result}")

    # Conectar con la API de GitHub
    g = Github(GITHUB_TOKEN)
    repo = g.get_repo(REPO_NAME)
    issue = repo.get_issue(number=ISSUE_NUMBER)

    # 1. Aplicar Etiquetas (Labels)
    labels_to_apply = [lbl for lbl in ai_result.get("labels", []) if lbl in AVAILABLE_LABELS]
    if labels_to_apply:
        issue.add_to_labels(*labels_to_apply)
        print(f"Etiquetas aplicadas: {labels_to_apply}")

    # 2. Asignar Colaborador
    assignee = ai_result.get("assignee")
    if assignee and assignee in TEAM_MEMBERS:
        issue.add_to_assignees(assignee)
        print(f"Asignado a: {assignee}")

    # 3. Publicar comentario diagnóstico
    comment_text = ai_result.get("comment", "")
    if comment_text:
        body_comment = f" **Triaje Automático (JEV / OpenRouter):**\n\n{comment_text}"
        issue.create_comment(body_comment)
        print("Comentario de triaje publicado.")

if __name__ == "__main__":
    main()