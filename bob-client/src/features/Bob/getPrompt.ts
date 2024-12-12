import markdownPrompts from 'prompts.md';

export const getPrompt = (heading: string): string => {
    const regex = new RegExp(`####\\s*${heading}\\s*([^#]+)`, 'i');

    const match = markdownPrompts.match(regex);
    if (!match) {
        console.error(`Prompt with heading "${heading}" not found`);
        return '';
    }

    return match[1].trim();
};
