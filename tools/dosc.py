#!/usr/bin/env python3
"""
Documentation Generator for BloodG OS
Generates documentation from source code comments
"""

import os
import re
from pathlib import Path
from datetime import datetime

class DocumentationGenerator:
    def __init__(self, project_root="."):
        self.project_root = Path(project_root)
        self.sections = {}
        
    def parse_source_file(self, file_path):
        """Parse source file for documentation comments"""
        docs = {
            'file': str(file_path),
            'description': '',
            'functions': [],
            'structs': [],
            'constants': [],
        }
        
        try:
            with open(file_path, 'r') as f:
                content = f.read()
                
            # Extract file description (first multi-line comment)
            desc_match = re.search(r'/\*\*\s*(.*?)\s*\*/', content, re.DOTALL)
            if desc_match:
                docs['description'] = desc_match.group(1).strip()
            
            # Extract functions
            func_pattern = r'/\*\*\s*(.*?)\s*\*/\s*\w+\s+\*?\s*(\w+)\s*\((.*?)\)'
            for match in re.finditer(func_pattern, content, re.DOTALL):
                func_doc = match.group(1).strip()
                func_name = match.group(2).strip()
                func_params = match.group(3).strip()
                
                docs['functions'].append({
                    'name': func_name,
                    'description': func_doc,
                    'params': func_params,
                })
            
            # Extract structs
            struct_pattern = r'struct\s+(\w+)\s*\{([^}]+)\}'
            for match in re.finditer(struct_pattern, content):
                struct_name = match.group(1)
                struct_body = match.group(2)
                
                # Parse struct members
                members = []
                for line in struct_body.split(';'):
                    line = line.strip()
                    if line:
                        parts = line.split()
                        if len(parts) >= 2:
                            member_type = ' '.join(parts[:-1])
                            member_name = parts[-1]
                            members.append({
                                'type': member_type,
                                'name': member_name,
                            })
                
                docs['structs'].append({
                    'name': struct_name,
                    'members': members,
                })
            
            # Extract #define constants
            define_pattern = r'#define\s+(\w+)\s+(.+)'
            for match in re.finditer(define_pattern, content):
                const_name = match.group(1)
                const_value = match.group(2).strip()
                
                docs['constants'].append({
                    'name': const_name,
                    'value': const_value,
                })
                
        except Exception as e:
            print(f"Error parsing {file_path}: {e}")
        
        return docs
    
    def generate_markdown(self):
        """Generate Markdown documentation"""
        output = []
        
        # Header
        output.append("# BloodG OS Documentation")
        output.append(f"*Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}*")
        output.append("")
        
        # Table of Contents
        output.append("## Table of Contents")
        output.append("")
        output.append("1. [Project Structure](#project-structure)")
        output.append("2. [Boot Process](#boot-process)")
        output.append("3. [Kernel API](#kernel-api)")
        output.append("4. [Memory Layout](#memory-layout)")
        output.append("5. [Development Tools](#development-tools)")
        output.append("")
        
        # Project Structure
        output.append("## Project Structure")
        output.append("")
        output.append("```")
        
        for root, dirs, files in os.walk(self.project_root):
            level = root.replace(str(self.project_root), '').count(os.sep)
            indent = ' ' * 2 * level
            rel_path = os.path.relpath(root, self.project_root)
            
            if rel_path == '.':
                output.append(f"{indent}./")
            else:
                output.append(f"{indent}{os.path.basename(root)}/")
            
            subindent = ' ' * 2 * (level + 1)
            for file in files:
                if file.endswith(('.c', '.asm', '.py', '.ld', 'Makefile', '.sh')):
                    output.append(f"{subindent}{file}")
        
        output.append("```")
        output.append("")
        
        # Parse source files
        source_files = []
        for ext in ['.c', '.asm']:
            source_files.extend(self.project_root.glob(f"**/*{ext}"))
        
        for source_file in source_files:
            if source_file.name in ['memory_check.c', 'docs.py']:
                continue
                
            docs = self.parse_source_file(source_file)
            rel_path = source_file.relative_to(self.project_root)
            
            output.append(f"## {rel_path}")
            output.append("")
            
            if docs['description']:
                output.append(docs['description'])
                output.append("")
            
            if docs['functions']:
                output.append("### Functions")
                output.append("")
                for func in docs['functions']:
                    output.append(f"#### `{func['name']}`")
                    output.append("")
                    output.append(f"{func['description']}")
                    output.append("")
                    if func['params']:
                        output.append(f"**Parameters:** `{func['params']}`")
                        output.append("")
            
            if docs['structs']:
                output.append("### Data Structures")
                output.append("")
                for struct in docs['structs']:
                    output.append(f"#### `struct {struct['name']}`")
                    output.append("```c")
                    output.append(f"struct {struct['name']} {{")
                    for member in struct['members']:
                        output.append(f"    {member['type']} {member['name']};")
                    output.append("};")
                    output.append("```")
                    output.append("")
            
            if docs['constants']:
                output.append("### Constants")
                output.append("")
                for const in docs['constants']:
                    output.append(f"- `{const['name']}` = `{const['value']}`")
                output.append("")
        
        return '\n'.join(output)
    
    def save_documentation(self, output_file="DOCUMENTATION.md"):
        """Save generated documentation to file"""
        markdown = self.generate_markdown()
        
        with open(output_file, 'w') as f:
            f.write(markdown)
        
        print(f"Documentation saved to {output_file}")
        return output_file

def main():
    """Main function"""
    generator = DocumentationGenerator()
    
    print("Generating BloodG OS documentation...")
    
    output_file = generator.save_documentation()
    
    print(f"✅ Documentation generated: {output_file}")
    print(f"📄 {os.path.getsize(output_file)} bytes")
    
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())
